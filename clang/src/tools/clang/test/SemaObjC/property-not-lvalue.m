// RUN: %clang_cc1 -fsyntax-only -verify %s

typedef struct NSSize {
     		int width;
		struct {
		  int dim;
		} inner;
} NSSize;

@interface Foo  {
        NSSize _size;
}
@property NSSize size;
@end

void foo() { 
        Foo *f;
        f.size.width = 2.2; // expected-error {{expression is not assignable using property assignment syntax}}
	f.size.inner.dim = 200; // expected-error {{expression is not assignable using property assignment syntax}}
}

// radar 7628953

@interface Gorf  {
}
- (NSSize)size;
@end

@implementation Gorf
- (void)MyView_sharedInit {
    self.size.width = 2.2; // expected-error {{expression is not assignable using property assignment syntax}}
}
- (NSSize)size {}
@end
