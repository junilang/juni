typedef struct {
	atomic_uint_fast32_t count;
} ArcState;

#define Arc_BITS 2

typedef enum : u8 {
	ArcClass_OWNER,
	ArcClass_WEAK,
	ArcClass_CONST
} ArcClass;

#define Arc_ALIGN (\
	(1 << Arc_BITS) > alignof(ArcState) ? \
	(1 << Arc_BITS) : alignof(ArcState) \
)

#define Arc_MEMBER alignas(Arc_ALIGN) ArcState

typedef UNIQUEPTR(Arc);

Arc ZZArc_setclass(ArcState *state, ArcClass class) {
	return lptrtag(state, Arc_BITS, class);
}

#define Arc_CONST ZZArc_setclass(nullptr, ArcClass_CONST)

ArcState *Arc_state(Arc rc) {
	return lptrstrip(rc, Arc_BITS);
}

ArcClass Arc_class(Arc rc) {
	return (ArcClass)lptrread(rc, Arc_BITS);
}

Arc Arc_init(ArcState *state) {
	atomic_init(&state->count, 1);
	state->count = 1;
	return ZZArc_setclass(state, ArcClass_OWNER);
}

Arc Arc_create(Allocator alc) {
	ArcState *state = Allocator_new(alc, sizeof(ArcState));
	return Arc_init(state);
}

bool Arc_release(Arc rc) {
	switch (Arc_class(rc)) {
		case ArcClass_CONST:
		case ArcClass_WEAK:
			return false;
		case ArcClass_OWNER:
			ArcState *state = Arc_state(rc);
			return atomic_fetch_sub(&state->count, 1) == 1;
	} UNREACHABLE;
}

bool Arc_destroy(Arc rc, Allocator alc) {
	switch (Arc_class(rc)) {
		case ArcClass_CONST:
		case ArcClass_WEAK:
			return false;
		case ArcClass_OWNER:
			ArcState *state = Arc_state(rc);
			if (atomic_fetch_sub(&state->count, 1) != 1)
				return false;

			Allocator_delete(alc, state);
			return true;
	} UNREACHABLE;
}

Arc Arc_copy(Arc rc) {
	switch (Arc_class(rc)) {
		case ArcClass_CONST:
			return rc;
		case ArcClass_WEAK:
		case ArcClass_OWNER:
			ArcState *state = Arc_state(rc);
			atomic_fetch_add(&state->count, 1);
			return ZZArc_setclass(state, ArcClass_OWNER);
	} UNREACHABLE;
}

Arc Arc_weak(Arc rc) {
	switch (Arc_class(rc)) {
		case ArcClass_CONST:
		case ArcClass_WEAK:
			return rc;
		case ArcClass_OWNER:
			return ZZArc_setclass(Arc_state(rc), ArcClass_WEAK);
	} UNREACHABLE;
}

Arc Arc_const(Arc rc) {
	switch (Arc_class(rc)) {
		case ArcClass_CONST:
		case ArcClass_WEAK:
			return rc;
		case ArcClass_OWNER:
			return ZZArc_setclass(Arc_state(rc), ArcClass_WEAK);
	} UNREACHABLE;
}

bool Arc_isconst(Arc rc) { return Arc_class(rc) == ArcClass_CONST; }
bool Arc_isweak(Arc rc) { return Arc_class(rc) == ArcClass_WEAK; }
bool Arc_isowner(Arc rc) { return Arc_class(rc) == ArcClass_OWNER; }
