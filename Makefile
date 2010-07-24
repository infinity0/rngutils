CFLAGS=-lrt
ADDEN=addentropy
GENUS=rng-usleep

all: rng-usleep addentropy

clean:
	rm -rf rng-usleep addentropy
