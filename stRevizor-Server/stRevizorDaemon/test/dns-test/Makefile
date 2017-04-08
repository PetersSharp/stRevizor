CFLAGS = -g -O2 -std=c99
st-revizor-dns-test-main: \
    st-revizor-dns-test-main.c \
    st-revizor-dns-test-request.c \
    ../../src/filter/st-revizor-lock.c \
    ../../src/util/thpool.c \
    -lpthread -lm

clean:
	rm -f st-revizor-dns-test-main
