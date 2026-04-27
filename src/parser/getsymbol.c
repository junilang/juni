Symbol Parser_getsymbol(ParserContext *ctx, Trie symdb, SourceRef src, SourcePos *pos) {
	StringSpan buf = Source_at(src, pos->offset);

	const ubyte *it = buf.begin;
	const ubyte *symbol_end = nullptr;
	Symbol symbol = Symbol_NULL;

	Trie caret = symdb;

	while (true) {
		const Ptr vdata = Trie_data(caret);
		if (Trie_isoccupied(caret)) {
			symbol_end = it;
			symbol = (Symbol){*(Ptr*)vdata};
		}

		const usize buf_size = (usize)(buf.end - it);
		if (!buf_size) break;

		if (Trie_isbranch(caret)) {
			const TrieBranch *data = vdata;

			ubyte c = *(it++);

			u8 map_idx = c / 64;
			u64 map_bit = (u64)1 << (c % 64);

			if (data->map[map_idx] & map_bit) {
				uint i = ZZTrieBranch_index(data, map_idx, map_bit);
				caret = data->next[i];
				continue;
			} else {
				break;
			}
		} else {
			const TrieSegment *data = vdata;

			Trie next = data->next;
			const usize size = data->size;

			if (
				Trie_isnull(next) ||
				(size > buf_size) ||
				memcmp(data->bytes, it, size)
			) {
				break;
			} else {
				it += size;
				caret = next;
				continue;
			}
		}
	}

	#if true
		PDBG(PDBG_BEGIN, "Symbol(", SymbolClass_Repr[Symbol_class(symbol)], ") ")
		if (symbol_end) {
			PDBG("\"", VString_upcast(
				buf.begin, (usize)(symbol_end - buf.begin),
				FLAG(VStringFlag, KEEPUTF)
			), "\" ");
		}
		PDBG(Source_name(src),":",pos->row,":",pos->col);
		PDBG(PDBG_END);
	#endif

	if (symbol_end) {
		*pos = SourcePos_advance(*pos, buf.begin, symbol_end);
	}

	return symbol;
}
