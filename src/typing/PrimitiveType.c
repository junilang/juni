ENUM_GENERATE(PrimitiveType, u8,
	Int
)

SinkRes PrimitiveType_fmt(PrimitiveType this, FmtSpec spec, Sink sink) {
	auto const s = PrimitiveType_str[this];
	return Sink_write(sink, s.data, s.size);
}

IFmt_GENERATE(PrimitiveType, PrimitiveType)

Fmt PrimitiveType_asTypeFmt(TypePtr vthis) {
	return PrimitiveType_asFmt((PrimitiveType)TypePtr_enum(vthis));
}

bool PrimitiveType_lmatch(TypePtr vthis, Type other) {
	return false;
}

bool PrimitiveType_rmatch(TypePtr vthis, Type other) {
	return false;
}

IType_GENERATE_ENUM_KNOWN(PrimitiveType, PrimitiveType)
