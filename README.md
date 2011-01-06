# RNG utils


## Building

To build the package, simply run `make`.

The only `-l` option to `gcc` is `-lrt`, which was already installed on my
fresh Debian squeeze system. If you get errors, you might need to upgrade your
system. (Sorry for the lack of more detailed info; I don't have time to look
into this right now.)

## Testing

**Testing the output is very important**. Since this package is software and
therefore deterministic, all the randomness that it gathers will be from its
environment. The assumptions about this (such as the accuracy of clocks) may
not hold on your particular setup, so **you must test the output yourself**.

(TODO make testing an automatic part of `addentropy.c`...)

Some suggested test suites:

###FIPS 140-2
These tests are relatively simple and unsophisticated. If your data fails
these, then it's most likely not random. `rng-tools`[^DRT] implements this
as a standalone utility `rngtest`, which makes for convenient testing.

###[ENT][^ENT]
These tests are somewhat more complex. The test suite doesn't give any pass
or fail boundaries, but you can get an idea of its relative performance by
comparing the evaluation of your candidate data, to the evaluation of the
same amount of data read from `/dev/urandom`.

###[Diehard][^DHD]
Haven't tried this out yet TODO.

[^DRT]: http://packages.debian.org/sid/rng-tools
[^ENT]: http://www.fourmilab.ch/random/
[^DHD]: http://en.wikipedia.org/wiki/Diehard%20tests


## Usage

Currently, the package is structured into several programs. Each RNG is a
separate program which generates data to stdout. To actually add this to the
kernel entropy pool, use `addentropy`, eg:

 $ ./rng-usleep | sudo ./addentropy
 generating random bytes @ 40+c us/bit

This is so the output of any particular RNG can be easily redirected into a
file or piped to a testing program. If you want to see the data rate, install
`pv` (Pipe Viewer) and place it in the middle of the pipe:

  $ ./rng-usleep | pv | sudo ./addentropy
  generating random bytes @ 40+c us/bit
  13.2kB 0:00:15 [ 988B/s ] [      <=>                                          ]

Use `addentropy` with caution; it assumes that its incoming data has 1 bit of
entropy per real bit. The TRNGs in this package will output such data, if their
assumptions hold on their execution environment.

ENT (mentioned above) can tell you the entropy rate of a sample of test data;
to get a more accurate result, feed it more data.


## TRNGs

This section contains documentation for the TRNGs implemented by this package.

### rng-usleep

We extract entropy from variations in the actual time slept by usleep(8). This
is determined largely by the kernel scheduler (clock drift barely affects it),
so don't use this if an attacker can feasibly predict it.

Disclaimer: this is considered a poor TRNG design, because schedulers are not
intrinsically random. The entropy source (the execution environment) cannot be
modelled easily, and therefore it's hard to prove results about it. However,
the practical output of a well-implemented clock-based TRNG will generally fare
well on existing randomness tests.

This algorithm tries to be prudent, and only generates 1 entropy bit per sleep
interval. It XORs two clock reads either side of the interval, then counts the
number of bits set and returns its LSB. (TODO do an analysis of this...)

Given a high-entropy execution environment, the output should pass FIPS 140-2,
and do well on all the ENT tests. On my machine (Debian Linux 2.6.32-5-amd64),
it can consistently score inside the 25%-75% range on the chi-square test.

(This was inspired by [timer entropy daemon][^TED], which generates entropy in
an inefficient way, and whose output fails FIPS 140-2.)

[^TED]: http://www.vanheusden.com/te/

