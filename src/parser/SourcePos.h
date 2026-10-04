typedef struct {
	usize offset;
	u32 row;
	u32 col;
} SourcePos;

constexpr SourcePos SourcePos_zero = { .offset = 0, .row = 1, .col = 1 };

SourcePos SourcePos_advance(SourcePos pos, const u8 *it, const u8 *end) {
	pos.offset = (usize)(end - it);

	for (; it < end; it++) {
		const u8 c = *it;
		if (c == 10) { // line feed
			pos.col = 1;
			pos.row++;
		} else if (
			c == 9 || 					// horizontal tab
			(c >= 32 && c < 127) || // printable ascii
			c >= 192 					// lead utf8 byte
		) {
			pos.col++;
		}
	}

	return pos;
}
