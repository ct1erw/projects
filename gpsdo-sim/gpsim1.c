//
// gpsim1 -- Simple GPSDO simulation.
//
// - Input file #1 is real GPS 1PPS phase data
// - Input file #2 is real XO / TCXO/ OCXO / Rb or Cs phase data
// - Output is simulated GPSDO result (phase data)
//
// - Assumes 1 Hz data rate for all files.
// - Use TimeLab to plot phase, frequency, and stability (Allan deviation) of all three files.
// - This gets very interesting with several types of GPS data, several types of OCXO data.
// - Nice testbed for parameter determination or alternative disciplining algorithms.
//
// 10-Dec-2008 Tom Van Baak (tvb) www.LeapSecond.com/tools
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double *path_to_double (char *path, long *count);
void detrend (double *data, long n, double *slope, double *offset);
double pid (double err, double kP, double kI, double kD);
double piid (double err, double kP, double kI, double kII, double kD);
double tic_resolution (double tic, double ticres);
double efc_resolution (double efc, double dacbits);

char *Tool;
int Verbose = 1;
#define FPRINTF Verbose && fprintf

int main (int argc, char *argv[])
{
    double *gps, *osc;
    long i, gps_lines, osc_lines;
    double osc_phase, tic, tic_filter, slope, offset, efc_filter, efc;
    char *set_fmt = "** setting %s = %lg\n";
    char *show_fmt = "** %7s = %lg\n";

    // Set parameter defaults.

    double ver = 2;         // algorithm version
    double n = 0.0;         // sample count
    double kp = 1e-3;       // PID: proportional
    double ki = 1e-7;       // PID: integral
    double kii = 0;         // PIID: 2nd integral
    double kd = 0;          // PID: derivative
    double dt = 1.0;        // sample interval (seconds)
    double gain = 1e-7;     // OCXO EFC gain (Hz/Hz / volt)
    double avg1 = 100;      // filter: GPS short-term
    double avg2 = 10000;    // filter: EFC long-term
    double ticres = 0.0;    // simulated finite TIC resolution
    double dacbits = 0.0;   // simulated finite DAC resolution

    Tool = argv[0];
    if (argc > 1 && strcmp(argv[1], "/q") == 0) {
        Verbose = 0;
        { argc -= 1; argv += 1; }
    }

    // Let user change parameters.

#define SET_PARAM(x) if (sscanf(argv[1], #x "=%lf", &x) == 1) { FPRINTF(stderr, set_fmt, #x, x); } else
#define UNKNOWN_PARAM  { fprintf(stderr, "%s: unknown parameter: %s\n", Tool, argv[1]); exit(1); }

    while (argc > 1 && strstr(argv[1], "=") != NULL) {
        SET_PARAM(ver)
        SET_PARAM(n)
        SET_PARAM(kp)
        SET_PARAM(ki)
        SET_PARAM(kii)
        SET_PARAM(kd)
        SET_PARAM(dt)
        SET_PARAM(gain)
        SET_PARAM(avg1)
        SET_PARAM(avg2)
        SET_PARAM(ticres)
        SET_PARAM(dacbits)
        UNKNOWN_PARAM
        { argc -= 1; argv += 1; }
    }

    if (argc != 3) {
        fprintf(stderr, "Usage: %s [parameter] gps.dat osc.dat > gpsdo.dat\n", Tool);
        exit(1);
    }

    // Sanity checks.

    if (avg1 < 1) { avg1 = 1; }
    if (avg2 < 1) { avg2 = 1; }

    // Read both phase measurement files.

    gps = path_to_double(argv[1], &gps_lines);
    FPRINTF(stderr, "** GPS %ld samples in file %s\n", gps_lines, argv[1]);

    osc = path_to_double(argv[2], &osc_lines);
    FPRINTF(stderr, "** OSC %ld samples in file %s\n", osc_lines, argv[2]);

    // If sample count unspecified, use smaller of both files.

    if (n == 0.0) {
        n = (gps_lines < osc_lines) ? gps_lines : osc_lines;
    }
    if (n > gps_lines || n > osc_lines) {
        fprintf(stderr, "%s: sample count specified (%ld) exceeds data counts (%ld, %ld)\n", Tool, (long) n, gps_lines, osc_lines);
        exit(1);
    }

    // For convenience, detrend and normalize the GPS data file.

    detrend(gps, (long) n, &slope, &offset);
    FPRINTF(stderr, "** GPS slope = %lg, removed\n", slope);
    FPRINTF(stderr, "** GPS offset = %lg, removed\n", offset);

    // Echo operating parameters.

#define SHOW_PARAM(x) FPRINTF(stderr, show_fmt, #x, x)
    SHOW_PARAM(ver);
    SHOW_PARAM(n);
    SHOW_PARAM(kp);
    SHOW_PARAM(ki);
    SHOW_PARAM(kii);
    SHOW_PARAM(kd);
    SHOW_PARAM(gain);
    SHOW_PARAM(avg1);
    SHOW_PARAM(avg2);
    if (ticres != 0.0)  { SHOW_PARAM(ticres); }
    if (dacbits != 0.0) { SHOW_PARAM(dacbits); }

    // Simulate virtual TIC-based GPSDO using real OSC and GPS data, once a second.

    osc_phase = 0.0;
    tic_filter = 0.0;   // used to smooth GPS noise
    efc_filter = 0.0;   // used to handle slow frequency drift
    efc = efc_filter;
    printf("%.6le\n", osc_phase);

    for (i = 1; i < (long) n; i += 1) {

        if (ver == 1) {

            // Simple PID.

            double freq = efc * gain;
            double jitter = osc[i] - osc[i - 1];
            double gps_phase = gps[i];
            osc_phase += (freq * dt) + jitter;
            tic = gps_phase - osc_phase;    // positive values mean oscillator is behind (slow)
            efc = pid(tic, kp, ki, kd) / gain;

        } else if (ver == 2) {

            // PID, with tic filter, slowly self-adjusting set point, finite resolution hack.

            double freq = efc * gain;
            double jitter = osc[i] - osc[i - 1];
            double gps_phase = gps[i];
            osc_phase += (freq * dt) + jitter;
            tic = gps_phase - osc_phase;    // positive values mean oscillator is behind (slow)
            tic = tic_resolution(tic, ticres);
            tic_filter = (tic_filter * (avg1 - 1) + tic) / avg1;
            efc = efc_filter + pid(tic_filter, kp, ki, kd) / gain;
            efc_filter = (efc_filter * (avg2 - 1) + efc) / avg2;
            efc = efc_resolution(efc, dacbits);

        } else if (ver == 3) {

            // PIID, with tic filter, finite resolution hack.

            double freq = efc * gain;
            double jitter = osc[i] - osc[i - 1];
            double gps_phase = gps[i];
            osc_phase += (freq * dt) + jitter;
            tic = gps_phase - osc_phase;    // positive values mean oscillator is behind (slow)
            tic = tic_resolution(tic, ticres);
            tic_filter = (tic_filter * (avg1 - 1) + tic) / avg1;
            efc = piid(tic_filter, kp, ki, kii, kd) / gain;
            efc = efc_resolution(efc, dacbits);

        } else {
            fprintf(stderr, "%s: algorithm version %lg not known\n", Tool, ver);
            exit(1);
        }

        printf("%.6le\n", osc_phase);

        if (fabs(tic) > 1e-4) {
            fprintf(stderr, "%s: TIC exceeds 100 usec; use tighter PID parameters?\n", Tool);
            exit(1);
        }
    }
    return 0; // exit(0);
}

// Implement PID algorithm.
double pid (double err, double kP, double kI, double kD)
{
    double d_err;
    static double sum_err = 0.0;
    static double prev_err = 0.0;

    sum_err += err;
    d_err = err - prev_err;
    prev_err = err;
    // printf(" %lg %lg %lg pid ", kP * err, kI * sum_err, kD * d_err);
    return (kP * err) + (kI * sum_err) + (kD * d_err);
}

// Implement PIID algorithm.
double piid (double err, double kP, double kI, double kII, double kD)
{
    double d_err;
    static double sum_err = 0.0;
    static double sum_sum_err = 0.0;
    static double prev_err = 0.0;

    sum_err += err;
    sum_sum_err += sum_err;
    d_err = err - prev_err;
    prev_err = err;
    // printf(" %lg %lg %lg %lg piid ", kP * err, kI * sum_err, kI * sum_sum_err, kD * d_err);
    return (kP * err) +  (kI * sum_err) + (kII * sum_sum_err) + (kD * d_err);
}

// Simulate finite TIC resolition or quantization.
double tic_resolution (double tic, double ticres)
{
    if (ticres > 0.0) {
        double sign = (tic < 0.0) ? -1 : 1;
        long count = (long) (fabs(tic) / ticres + 0.5);
        // printf(" %ld tic ", count);
        tic = sign * count * ticres;
    }
    return tic;
}

// Simulate finite DAC resolition or quantization.
double efc_resolution (double efc, double dacbits)
{
    if (dacbits > 0.0) {
        double range = 10.0;        // Assume +5 to -5 volts full scale
        double lsb = range / pow(2.0, dacbits);
        double sign = (efc < 0.0) ? -1 : 1;
        long count = (long) (fabs(efc) / lsb + 0.5);
        // printf(" %ld efc ", count);
        efc = sign * count * lsb;
    }
    return efc;
}

// Read file into array of doubles.
double *file_to_double (FILE *file, long *count)
{
    char line[1000];
    double *data;
    long i, imax, size;

    imax = 512;
    data = malloc(size = imax * sizeof data[0]);

    for (i = 0; fgets(line, sizeof line, file) != NULL; i += 1) {
        if (i == imax) {
            imax *= 2;
            data = realloc(data, size = imax * sizeof data[0]);
        }
        if (NULL == data) {
            fprintf(stderr, "%s: out of memory (%ld)\n", Tool, size);
            exit(1);
        }
        data[i] = atof(line);
    }
    *count = i;
    return data;
}

// Open given path (or stdin) and return auto-allocated double[] array with entire data set.
double *path_to_double (char *path, long *count)
{
    double *data;
    FILE *file;
    long n;
    char *name;
    static int once = 0;

    if (strcmp(path, "-") == 0 && once++ == 0) {
        file = stdin;
        name = "(stdin)";

    } else {
        if ((file = fopen(path, "r")) == NULL) {
            fprintf(stderr, "%s: open failed: %s\n", Tool, path);
            exit(1);
        }
        name = path;
    }
    data = file_to_double(file, &n);
    if (file != stdin) {
        fclose(file);
    }
    if (n < 2) {
        fprintf(stderr, "%s: %s: empty or too small (%ld)\n", Tool, name, n);
        exit(1);
    }
    *count = n;
    return data;
}

// Remove mean and slope from data set (linear least squares fit).
void detrend (double *data, long n, double *slope, double *offset)
{
    long x;
    double dx, dy, sum_x, sum_y, sum_dx2, sum_dxdy, xmean, ymean;

    sum_x = sum_y = 0.0;
    for (x = 0; x < n; x += 1) {
        sum_x += x;
        sum_y += data[x];
    }
    xmean = sum_x / n;
    ymean = sum_y / n;

    sum_dx2 = sum_dxdy = 0.0;
    for (x = 0; x < n; x += 1) {
        dx = x - xmean;
        dy = data[x] - ymean;
        sum_dx2  += dx * dx;
        sum_dxdy += dx * dy;
    }
    *slope = sum_dxdy / sum_dx2;
    *offset = ymean - *slope * xmean;

    // Detrend (y = mx + b) and replace data with residuals.
    for (x = 0; x < n; x += 1) {
        double y = *slope * x + *offset;
        data[x] -= y;
    }
}
