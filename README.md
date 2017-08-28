# Guetzli Android

## 简介

此Android Studio工程系基于[ianhuang-777的Guetzli仓库][ianhuang-777_guetzli]的Android移植版，在保留Google原版Guetzli实现的前提下，实现了优化版和OpenCL版在Android上的运行。

## 使用方法

在`MainActivity`中调用`compressImage()`时可以指定运行哪种模式的Guetzli。

当运行OpenCL版时，需要提前在外部存储目录（`/storage/emulated/0/`）下新建clguetzli文件夹，其中包含clguezli.cl文件。在工程asserts目录下有红米Note 4X（高通版）可用的clguetzli.cl文件。

## 已知问题

1. 由于不同设备使用的OpenCL实现不同，所以提供的clguetzli.cl文件可能不适用于其他设备。

[ianhuang-777_guetzli]: https://github.com/ianhuang-777/guetzli
