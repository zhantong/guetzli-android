/*
* OpenCL/CUDA edition implementation of ButteraugliComparator.
*
* Author: strongtu@tencent.com
*         ianhuang@tencent.com
*         chriskzhou@tencent.com
*/
#include <algorithm>
#include <stdint.h>
#include <vector>
#include "clguetzli.h"
#include "clguetzli.cl"
#include "utils.h"
#define abs(exper)    fabs((exper))
#define __checkcl


#ifdef __USE_OPENCL__

using namespace std;

int g_idvec[10] = { 0 };
int g_sizevec[10] = { 0 };

int get_global_id(int dim) {
    return g_idvec[dim];
}
int get_global_size(int dim) {
    return g_sizevec[dim];
}

void set_global_id(int dim, int id){
    g_idvec[dim] = id;
}
void set_global_size(int dim, int size){
    g_sizevec[dim] = size;
}


namespace guetzli
{
    ButteraugliComparatorEx::ButteraugliComparatorEx(const int width, const int height,
        const std::vector<uint8_t>* rgb,
        const float target_distance, ProcessStats* stats)
        : ButteraugliComparator(width, height, rgb, target_distance, stats)
    {
        if (MODE_CPU != g_mathMode)
        {
            rgb_orig_opsin.resize(3);
            rgb_orig_opsin[0].resize(width * height);
            rgb_orig_opsin[1].resize(width * height);
            rgb_orig_opsin[2].resize(width * height);

#ifdef __USE_DOUBLE_AS_FLOAT__
            const float* lut = kSrgb8ToLinearTable;
#else
            const double* lut = kSrgb8ToLinearTable;
#endif
            for (int c = 0; c < 3; ++c) {
                for (int y = 0, ix = 0; y < height_; ++y) {
                    for (int x = 0; x < width_; ++x, ++ix) {
                        rgb_orig_opsin[c][ix] = lut[rgb_orig_[3 * ix + c]];
                    }
                }
            }
            ::butteraugli::OpsinDynamicsImage(width_, height_, rgb_orig_opsin);
        }
    }

    void ButteraugliComparatorEx::Compare(const OutputImage& img)
    {
		if (MODE_CPU_OPT == g_mathMode)
		{
			std::vector<std::vector<float> > rgb0 = rgb_orig_opsin;

			std::vector<std::vector<float> > rgb(3, std::vector<float>(width_ * height_));
			img.ToLinearRGB(&rgb);
			::butteraugli::OpsinDynamicsImage(width_, height_, rgb);
			std::vector<float>().swap(distmap_);
			comparator_.DiffmapOpsinDynamicsImage(rgb0, rgb, distmap_);
			distance_ = ::butteraugli::ButteraugliScoreFromDiffmap(distmap_);
		}
#ifdef __USE_OPENCL__
        else if (MODE_OPENCL == g_mathMode)
        {
            std::vector<std::vector<float> > rgb1(3, std::vector<float>(width_ * height_));
            img.ToLinearRGB(&rgb1);

            const int xsize = width_;
            const int ysize = height_;
            std::vector<float>().swap(distmap_);
            distmap_.resize(xsize * ysize);

            size_t channel_size = xsize * ysize * sizeof(float);
            ocl_args_d_t &ocl = getOcl();
            ocl_channels xyb0 = ocl.allocMemChannels(channel_size, rgb_orig_opsin[0].data(), rgb_orig_opsin[1].data(), rgb_orig_opsin[2].data());
            ocl_channels xyb1 = ocl.allocMemChannels(channel_size, rgb1[0].data(), rgb1[1].data(), rgb1[2].data());

            cl_mem mem_result = ocl.allocMem(channel_size);

            clOpsinDynamicsImageEx(xyb1, xsize, ysize);
            clDiffmapOpsinDynamicsImageEx(mem_result, xyb0, xyb1, xsize, ysize, comparator_.step());

            cl_int err = clEnqueueReadBuffer(ocl.commandQueue, mem_result, false, 0, channel_size, distmap_.data(), 0, NULL, NULL);
            LOG_CL_RESULT(err);
            err = clFinish(ocl.commandQueue);
            LOG_CL_RESULT(err);

            clReleaseMemObject(mem_result);
            ocl.releaseMemChannels(xyb0);
            ocl.releaseMemChannels(xyb1);

            distance_ = ::butteraugli::ButteraugliScoreFromDiffmap(distmap_);
        }
#endif
#ifdef __USE_CUDA__
        else if (MODE_CUDA == g_mathMode)
        {
            std::vector<std::vector<float> > rgb1(3, std::vector<float>(width_ * height_));
            img.ToLinearRGB(&rgb1);

            const int xsize = width_;
            const int ysize = height_;
            std::vector<float>().swap(distmap_);
            distmap_.resize(xsize * ysize);

            size_t channel_size = xsize * ysize * sizeof(float);
            ocu_args_d_t &ocu = getOcu();
            ocu_channels xyb0 = ocu.allocMemChannels(channel_size, rgb_orig_opsin[0].data(), rgb_orig_opsin[1].data(), rgb_orig_opsin[2].data());
            ocu_channels xyb1 = ocu.allocMemChannels(channel_size, rgb1[0].data(), rgb1[1].data(), rgb1[2].data());
            
            cu_mem mem_result = ocu.allocMem(channel_size);

            cuOpsinDynamicsImageEx(xyb1, xsize, ysize);

            cuDiffmapOpsinDynamicsImageEx(mem_result, xyb0, xyb1, xsize, ysize, comparator_.step());

            cuMemcpyDtoH(distmap_.data(), mem_result, channel_size);

            ocu.releaseMem(mem_result);
            ocu.releaseMemChannels(xyb0);
            ocu.releaseMemChannels(xyb1);

            distance_ = ::butteraugli::ButteraugliScoreFromDiffmap(distmap_);
        }
#endif
		else
		{
			ButteraugliComparator::Compare(img);
		}
    }


    void ButteraugliComparatorEx::FinishBlockComparisons() {
        ButteraugliComparator::FinishBlockComparisons();

        imgOpsinDynamicsBlockList.clear();
        imgMaskXyzScaleBlockList.clear();
    }
    
    double ButteraugliComparatorEx::CompareBlock(const OutputImage& img, int off_x, int off_y, const coeff_t* candidate_block, const int comp_mask) const
    {
        double err = ButteraugliComparator::CompareBlock(img, off_x, off_y, candidate_block, comp_mask);
        return err;
    }
}

#endif