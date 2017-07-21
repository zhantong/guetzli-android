package com.ploarxiong.guetzli;

import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {
    String imagePath;
    public static final int REQUEST_CODE_ORIGIN_IMAGE_PATH = 1;

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button btnChooseImage = (Button) findViewById(R.id.btn_choose_image);
        btnChooseImage.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                chooseImage();
            }
        });

        Button btnStart = (Button) findViewById(R.id.btn_start);
        btnStart.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                start();
            }
        });
    }

    void chooseImage() {
        Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
        intent.setType("*/*");
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        startActivityForResult(intent, REQUEST_CODE_ORIGIN_IMAGE_PATH);
    }

    void start() {
        if (imagePath == null) {
            imagePath = "/storage/emulated/0/bees.png";
        }
        System.out.println("start");
        compressImage(imagePath, imagePath + "1");
        System.out.println("finish");
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (resultCode == RESULT_OK) {
            if (requestCode == REQUEST_CODE_ORIGIN_IMAGE_PATH) {
                Uri uri = data.getData();
                TextView textImagePath = (TextView) findViewById(R.id.text_image_path);
                imagePath = contentUriToFileUri(uri);
                textImagePath.setText(imagePath);
            }
        }
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native int compressImage(String inputImagePath, String outputImagePath);

    String contentUriToFileUri(Uri _uri) {
        String filePath;
        if (_uri != null && "content".equals(_uri.getScheme())) {
            Cursor cursor = getContentResolver().query(_uri, new String[]{android.provider.MediaStore.Images.ImageColumns.DATA}, null, null, null);
            cursor.moveToFirst();
            filePath = cursor.getString(0);
            cursor.close();
        } else {
            filePath = _uri.getPath();
        }
        return filePath;
    }
}
