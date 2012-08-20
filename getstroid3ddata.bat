echo "Downloading Stroids data files"
.\bin\wget www.cs.wisc.edu/~anthony/stroids3d_data.tar.gz
echo "Unzipping files"
.\bin\gzip -d stroids3d_data.tar.gz
.\bin\tar -xvvf stroids3d_data.tar
echo "Removing tar file"
del stroids3d_data.tar
