typedef struct {
	usize offset;
	u32 row;
	u32 col;
} SourcePos;

typedef struct {
	Arc_MEMBER rc;
	u16 name_size;
	Allocator alc;
	ubyte *name;
	usize data_size;
	ubyte data[];
} Source;

usize ZZSource_allocsize(usize size) {
	return offsetof(Source, data) + size;
}

typedef struct { Arc value; } SourceRef;

const Source *Source_data(SourceRef this) {
	return (Ptr)Arc_state(this.value);
}

StringSpan Source_at(SourceRef rthis, usize offset) {
	auto this = Source_data(rthis);

	#if BUILD_SAFE
		if (offset > this->data_size) PANIC("SourceRef_at: overflow");
	#endif

	return (StringSpan) {
		.begin = this->data + offset,
		.end = this->data + this->data_size
	};
}

SourceRef Source_copy(SourceRef rthis) {
	return (SourceRef){Arc_copy(rthis.value)};
}

SourceRef Source_const(SourceRef rthis) {
	return (SourceRef){Arc_const(rthis.value)};
}

void Source_release(SourceRef rthis) {
	if (Arc_release(rthis.value)) {
		auto this = Source_data(rthis);

		if (this->name)
			Allocator_delete(this->alc, this->name);

		Allocator_delete(this->alc, (Ptr)this);
	}
}

String Source_name(SourceRef rthis) {
	auto this = Source_data(rthis);
	return (String){.data=this->name,.size=this->name_size};
}


SourceRef Source_fromstring(String name, String data, Allocator alc) {
	Source *this = Allocator_new(alc, ZZSource_allocsize(data.size));
	this->name = name.size ? Allocator_new(alc, name.size) : nullptr;
	this->alc = alc;
	this->data_size = data.size;
	this->name_size = usize_pcast16(name.size);
	memcpy(this->data, data.data, data.size);
	memcpy(this->name, name.data, name.size);

	return (SourceRef){Arc_init(&this->rc)};
}

SourceRef Source_fromfile(String name, FILE *file, Allocator alc) {
	long tell_origin = ftell(file);
	if (tell_origin < 0) PANIC("ftell failed");

	if (fseek(file, 0, SEEK_END)) PANIC("fseek failed");

	long tell_size = ftell(file);
	if (tell_size < 0) PANIC("ftell failed");

	if (fseek(file, tell_origin, SEEK_SET)) PANIC("fseek failed");

	usize data_size = (usize)(tell_size - tell_origin);

	Source *this = Allocator_new(alc, ZZSource_allocsize((usize)data_size));

	if (fread(this->data, 1, data_size, file) != data_size) PANIC("read_size mismatch");
	if (fseek(file, tell_origin, SEEK_SET)) PANIC("fseek failed");

	this->alc = alc;
	this->data_size = data_size;
	this->name = name.size ? Allocator_new(alc, name.size) : nullptr;
	this->name_size = usize_pcast16(name.size);
	memcpy(this->name, name.data, name.size);

	return (SourceRef){Arc_init(&this->rc)};
}
