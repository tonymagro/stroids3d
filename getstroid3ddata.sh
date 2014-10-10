echo "Downloading Stroids data files"
curl -O http://www.cs.wisc.edu/~anthony/stroids3d_data.tar.gz
echo "Unzipping files"
tar -xzvvf stroids3d_data.tar.gz
echo "Removing tar file"
rm stroids3d_data.tar.gz
