#define SUCCESS			0

#define INFO			1

/* SYSTEMERROR means that there was a failure in storage allocation or i/o. */
#define SYSTEMERROR		2

/* COMMANDLINEERROR means that the user gave incorrect or inconsistent command line flags */
#define COMMANDLINEERROR	3

/*
 * RANGEERROR means that the input may be well-formed but cannot be represented
 * as the required type. For example, if the input is the string "983" and
 * ascii2binary is requested to convert this into an unsigned byte, ascii2binary
 * will exit with a RANGEERROR because the maximum value representable in an
 * unsigned byte is 255. This error is applicable only to ascii2binary.
*/
#define RANGEERROR		4

/*
 * INPUTERROR means that the input was ill-formed, that is, that it could not
 * be interpreted as a number of the required type.
 */
#define INPUTERROR		5

#define LOCALENOTAVAILABLE	6
