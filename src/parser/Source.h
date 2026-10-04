typedef struct {
	ArcState arc;
	Alc alc;
	ubyte *data_end;
	u16 name_size;
	ubyte data[];
} SourceData;

usize SourceData_ZZallocsize(usize data_size, u16 name_size) {
	return __builtin_offsetof(SourceData, data) + data_size + name_size;
}

typedef struct {
	Arc ref;
} Source;

const SourceData *Source_data(Source rthis) {
	return (const SourceData*)Arc_state(rthis.ref);
}

String Source_name(Source rthis) {
	auto this = Source_data(rthis);
	return (String) {
		.data = this->data_end,
		.size = (this->name_size)
	};
}

StringSpan Source_at(Source rthis, usize offset) {
	auto this = Source_data(rthis);

	#if BUILD_SAFE
		if (this->data + offset > this->data_end) PANIC("offset overflow");
	#endif

	return (StringSpan) {
		.begin = this->data + offset,
		.end = this->data_end
	};
}

AlcRes Source_release(Source rthis) {
	if (!Arc_release(rthis.ref)) return AlcRes_Ok;

	auto this = (SourceData*)Arc_state(rthis.ref);
	return Alc_delete(this->alc, this);
}

Source Source_copy(Source rthis) {
	return (Source){ .ref = Arc_copy(rthis.ref) };
}

Source Source_const(Source rthis) {
	return (Source){ .ref = Arc_const(rthis.ref) };
}

AlcRes Source_create(
	Alc alc,
	const ubyte *name, u16 name_size,
	const ubyte *data, usize data_size,
	Source *result
) {
	AlcReq req = {
		.intent = AlcIntent_New,
		.size = SourceData_ZZallocsize(data_size, name_size),
		.align = _Alignof(SourceData)
	};

	auto ptr = Alc_invoke(alc, &req, nullptr, nullptr);
	auto res = AlcPtr_get(ptr);
	if (res) return res;

	auto this = (SourceData*)ptr;
	auto data_end = this->data + data_size;
	this->alc = alc;
	this->name_size = name_size;
	this->data_end = data_end;
	memcpy(this->data, data, data_size);
	memcpy(data_end, name, name_size);

	*result = (Source) { .ref = Arc_init(&this->arc) };
	return AlcRes_Ok;
}
