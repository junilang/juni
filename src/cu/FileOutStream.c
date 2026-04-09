void FileOutStream_write(FILE *this, const ubyte *buffer, usize buffer_size) {
	fwrite(buffer, buffer_size, 1, this);
}

void FileOutStream_flush(FILE *this) {
	fflush(this);
}

IOutStream_GENERATE(FileOutStream)
IOutStream_REGISTER_KNOWN(FileOutStream)
