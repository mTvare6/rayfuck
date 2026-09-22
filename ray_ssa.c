#include <math.h>
#include <stdio.h>

// random
double rdstate;
double rdmore;
double rdout;

// randomunit
double randx;
double randy;
double randz;

// hit
double hitx;
double hity;
double hitz;
double hitnx;
double hitny;
double hitnz;
double hitt;
double hitmat;

// hitsphere
double hsfound;
double hsox;
double hsoy;
double hsoz;
double hsdx;
double hsdy;
double hsdz;
double hscx;
double hscy;
double hscz;
double hsr;
double hsmat;
double hstmin;
double hstmax;

// hitworld
double hwfound;
double hwox;
double hwoy;
double hwoz;
double hwdx;
double hwdy;
double hwdz;

// ray color
double rcox;
double rcoy;
double rcoz;
double rcdx;
double rcdy;
double rcdz;
double rcdepth;
double rcoutr;
double rcoutg;
double rcoutb;

void randomvalue(void) {
    rdstate = rdstate * 5;
    rdstate = rdstate + 1;
    rdmore = rdstate >= 256;
    while (rdmore) {
        rdstate = rdstate - 256;
        rdmore = rdstate >= 256;
    }
    rdout = rdstate / 256;
}

void randomunit(void) {
    double rux;
    double ruy;
    double ruz;
    double rut;
    double rulensq;
    double rulen;
    double rugt;
    double rule;
    double ruok;
    double runot;

    ruok = 0;
    runot = !ruok;
    while (runot) {
        randomvalue();
        rut = rdout * 2;
        rux = rut - 1;
        randomvalue();
        rut = rdout * 2;
        ruy = rut - 1;
        randomvalue();
        rut = rdout * 2;
        ruz = rut - 1;
        rut = rux * rux;
        rulensq = rut;
        rut = ruy * ruy;
        rulensq = rulensq + rut;
        rut = ruz * ruz;
        rulensq = rulensq + rut;
        rugt = rulensq > 0;
        rule = rulensq <= 1;
        ruok = rugt && rule;
        runot = !ruok;
    }
    rulen = sqrt(rulensq);
    randx = rux / rulen;
    randy = ruy / rulen;
    randz = ruz / rulen;
}

void hitsphere(void) {
    double hsoffx;
    double hsoffy;
    double hsoffz;
    double hsa;
    double hsh;
    double hsc;
    double hsdisc;
    double hsroot;
    double hssqrt;
    double hsoutx;
    double hsouty;
    double hsoutz;
    double hstmp;
    double hslow;
    double hshigh;
    double hsbad;
    double hsfront;

    hsoffx = hscx - hsox;
    hsoffy = hscy - hsoy;
    hsoffz = hscz - hsoz;
    hstmp = hsdx * hsdx;
    hsa = hstmp;
    hstmp = hsdy * hsdy;
    hsa = hsa + hstmp;
    hstmp = hsdz * hsdz;
    hsa = hsa + hstmp;
    hstmp = hsdx * hsoffx;
    hsh = hstmp;
    hstmp = hsdy * hsoffy;
    hsh = hsh + hstmp;
    hstmp = hsdz * hsoffz;
    hsh = hsh + hstmp;
    hstmp = hsoffx * hsoffx;
    hsc = hstmp;
    hstmp = hsoffy * hsoffy;
    hsc = hsc + hstmp;
    hstmp = hsoffz * hsoffz;
    hsc = hsc + hstmp;
    hstmp = hsr * hsr;
    hsc = hsc - hstmp;
    hstmp = hsh * hsh;
    hsdisc = hsa * hsc;
    hsdisc = hstmp - hsdisc;
    hsfound = hsdisc >= 0;
    if (hsfound) {
        hssqrt = sqrt(hsdisc);
        hsroot = hsh - hssqrt;
        hsroot = hsroot / hsa;
        hslow = hsroot <= hstmin;
        hshigh = hsroot >= hstmax;
        hsbad = hslow || hshigh;
        if (hsbad) {
            hsroot = hsh + hssqrt;
            hsroot = hsroot / hsa;
            hslow = hsroot <= hstmin;
            hshigh = hsroot >= hstmax;
            hsbad = hslow || hshigh;
        }
        hsfound = !hsbad;
        if (hsfound) {
            hitt = hsroot;
            hstmp = hsroot * hsdx;
            hitx = hsox + hstmp;
            hstmp = hsroot * hsdy;
            hity = hsoy + hstmp;
            hstmp = hsroot * hsdz;
            hitz = hsoz + hstmp;
            hitmat = hsmat;
            hsoutx = hitx - hscx;
            hsoutx = hsoutx / hsr;
            hsouty = hity - hscy;
            hsouty = hsouty / hsr;
            hsoutz = hitz - hscz;
            hsoutz = hsoutz / hsr;
            hstmp = hsdx * hsoutx;
            hsfront = hstmp;
            hstmp = hsdy * hsouty;
            hsfront = hsfront + hstmp;
            hstmp = hsdz * hsoutz;
            hsfront = hsfront + hstmp;
            hsfront = hsfront < 0;
            if (hsfront) {
                hitnx = hsoutx;
                hitny = hsouty;
                hitnz = hsoutz;
            } else {
                hitnx = -hsoutx;
                hitny = -hsouty;
                hitnz = -hsoutz;
            }
        }
    }
}

void hitworld(void) {
    double hwclose;

    hwclose = 32767;
    hwfound = 0;

    hsox = hwox;
    hsoy = hwoy;
    hsoz = hwoz;
    hsdx = hwdx;
    hsdy = hwdy;
    hsdz = hwdz;
    hscx = 0;
    hscy = -100.5;
    hscz = -1;
    hsr = 100;
    hsmat = 0;
    hstmin = 0.001;
    hstmax = hwclose;
    hitsphere();
    if (hsfound) {
        hwclose = hitt;
        hwfound = 1;
    }

    hscx = 0;
    hscy = 0;
    hscz = -1.2;
    hsr = 0.5;
    hsmat = 1;
    hstmax = hwclose;
    hitsphere();
    if (hsfound) {
        hwclose = hitt;
        hwfound = 1;
    }

    hscx = -1;
    hscy = 0;
    hscz = -1;
    hsr = 0.5;
    hsmat = 2;
    hstmax = hwclose;
    hitsphere();
    if (hsfound) {
        hwclose = hitt;
        hwfound = 1;
    }

    hscx = 1;
    hscy = 0;
    hscz = -1;
    hsr = 0.5;
    hsmat = 3;
    hstmax = hwclose;
    hitsphere();
    if (hsfound) {
        hwfound = 1;
    }
}

void raycolor(void) {
    // ray color 
    // hit point and normal
    double rcpx;
    double rcpy;
    double rcpz;
    double rcnx;
    double rcny;
    double rcnz;
    // scatter
    double rcsx;
    double rcsy;
    double rcsz;
    // attenuation
    double rcar;
    double rcag;
    double rcab;
    // reflection
    double rcfuzz;
    double rcrx;
    double rcry;
    double rcrz;
    double rcrlen;
    double rcproj;
    double rcdlen;
    double rcblend;
    // throughput
    double rctr;
    double rctg;
    double rctb;
    
    double rcmat;
    double rctype;
    double rcpath;
    double rcactive;
    double rcscatter;
    double rctmp;
    double rcsmallx;
    double rcsmally;
    double rcsmallz;
    double rcsmall;
    double rcmetal;
    double rcnot;
    double rceq;

    rcoutr = 0;
    rcoutg = 0;
    rcoutb = 0;
    rctr = 1;
    rctg = 1;
    rctb = 1;
    rcpath = 0;
    rcactive = rcpath < rcdepth;
    while (rcactive) {
        hwox = rcox;
        hwoy = rcoy;
        hwoz = rcoz;
        hwdx = rcdx;
        hwdy = rcdy;
        hwdz = rcdz;
        hitworld();
        if (hwfound) {
            rcpx = hitx;
            rcpy = hity;
            rcpz = hitz;
            rcnx = hitnx;
            rcny = hitny;
            rcnz = hitnz;
            rcmat = hitmat;
            rcscatter = 1;
            rcmetal = rcmat >= 2;
            rcnot = !rcmetal;
            if (rcnot) {
                rctype = 0;
                rceq = rcmat == 0;
                if (rceq) {
                    rcar = 0.8;
                    rcag = 0.8;
                    rcab = 0;
                } else {
                    rcar = 0.1;
                    rcag = 0.2;
                    rcab = 0.5;
                }
                rcfuzz = 0;
            } else {
                rctype = 1;
                rceq = rcmat == 2;
                if (rceq) {
                    rcar = 0.8;
                    rcag = 0.8;
                    rcab = 0.8;
                    rcfuzz = 0.3;
                } else {
                    rcar = 0.8;
                    rcag = 0.6;
                    rcab = 0.2;
                    rcfuzz = 1;
                }
            }
            rcnot = !rctype;
            if (rcnot) {
                randomunit();
                rcsx = rcnx + randx;
                rcsy = rcny + randy;
                rcsz = rcnz + randz;
                rctmp = fabs(rcsx);
                rcsmallx = rctmp < 0.000015;
                rctmp = fabs(rcsy);
                rcsmally = rctmp < 0.000015;
                rctmp = fabs(rcsz);
                rcsmallz = rctmp < 0.000015;
                rcsmall = rcsmallx && rcsmally;
                rcsmall = rcsmall && rcsmallz;
                if (rcsmall) {
                    rcsx = rcnx;
                    rcsy = rcny;
                    rcsz = rcnz;
                }
            } else {
                rctmp = rcdx * rcnx;
                rcproj = rctmp;
                rctmp = rcdy * rcny;
                rcproj = rcproj + rctmp;
                rctmp = rcdz * rcnz;
                rcproj = rcproj + rctmp;
                rctmp = rcproj * 2;
                rcrx = rctmp * rcnx;
                rcrx = rcdx - rcrx;
                rcry = rctmp * rcny;
                rcry = rcdy - rcry;
                rcrz = rctmp * rcnz;
                rcrz = rcdz - rcrz;
                rctmp = rcrx * rcrx;
                rcrlen = rctmp;
                rctmp = rcry * rcry;
                rcrlen = rcrlen + rctmp;
                rctmp = rcrz * rcrz;
                rcrlen = rcrlen + rctmp;
                rcrlen = sqrt(rcrlen);
                rcrx = rcrx / rcrlen;
                rcry = rcry / rcrlen;
                rcrz = rcrz / rcrlen;
                randomunit();
                rctmp = rcfuzz * randx;
                rcsx = rcrx + rctmp;
                rctmp = rcfuzz * randy;
                rcsy = rcry + rctmp;
                rctmp = rcfuzz * randz;
                rcsz = rcrz + rctmp;
                rctmp = rcsx * rcnx;
                rcproj = rctmp;
                rctmp = rcsy * rcny;
                rcproj = rcproj + rctmp;
                rctmp = rcsz * rcnz;
                rcproj = rcproj + rctmp;
                rcscatter = rcproj > 0;
            }
            if (rcscatter) {
                rctr = rctr * rcar;
                rctg = rctg * rcag;
                rctb = rctb * rcab;
                rcox = rcpx;
                rcoy = rcpy;
                rcoz = rcpz;
                rcdx = rcsx;
                rcdy = rcsy;
                rcdz = rcsz;
                rcpath = rcpath + 1;
                rcactive = rcpath < rcdepth;
            } else {
                rcactive = 0;
            }
        } else {
            rctmp = rcdx * rcdx;
            rcdlen = rctmp;
            rctmp = rcdy * rcdy;
            rcdlen = rcdlen + rctmp;
            rctmp = rcdz * rcdz;
            rcdlen = rcdlen + rctmp;
            rcdlen = sqrt(rcdlen);
            rctmp = rcdy / rcdlen;
            rctmp = rctmp + 1;
            rcblend = rctmp * 0.5;
            rctmp = rcblend * 0.5;
            rctmp = 1 - rctmp;
            rcoutr = rctr * rctmp;
            rctmp = rcblend * 0.3;
            rctmp = 1 - rctmp;
            rcoutg = rctg * rctmp;
            rcoutb = rctb;
            rcactive = 0;
        }
    }
}

int main(void) {
    // camera
    double maspect;
    double mwidth;
    double mheight;
    double msamples;
    double mdepth;
    double mviewh;
    double mvieww;
    double mstepx;
    double mstepy;
    double mpixelx;
    double mpixely;
    double mpixelz;
    // image
    double mtargetx;
    double mtargety;
    double mtargetz;
    double msampler;
    double msampleg;
    double msampleb;
    double mpixelr;
    double mpixelg;
    double mpixelb;
    double mx;
    double my;
    double msample;
    double moffx;
    double moffy;
    double mtmp;
    double mcond;
    
    // pixels
    double moutw;
    double mouth;
    double moutr;
    double moutg;
    double moutb;

    rdstate = 1;
    maspect = 16;
    maspect = maspect / 9;
    mwidth = 400;
    mheight = mwidth / maspect;
    mheight = (int)mheight;
    msamples = 100;
    mdepth = 50;
    mviewh = 2;
    mvieww = mviewh * mwidth;
    mvieww = mvieww / mheight;
    mstepx = mvieww / mwidth;
    mstepy = -mviewh;
    mstepy = mstepy / mheight;
    mpixelx = mvieww / 2;
    mpixelx = -mpixelx;
    mtmp = mstepx / 2;
    mpixelx = mpixelx + mtmp;
    mpixely = mviewh / 2;
    mtmp = mstepy / 2;
    mpixely = mpixely + mtmp;
    mpixelz = -1;
    moutw = (int)mwidth;
    mouth = (int)mheight;

    printf("P3\n");
    printf("%d %d\n255\n", (int)moutw, (int)mouth);

    my = 0;
    mcond = my < mheight;
    while (mcond) {
        mx = 0;
        mcond = mx < mwidth;
        while (mcond) {
            mpixelr = 0;
            mpixelg = 0;
            mpixelb = 0;
            msample = 0;
            mcond = msample < msamples;
            while (mcond) {
                randomvalue();
                moffx = rdout - 0.5;
                randomvalue();
                moffy = rdout - 0.5;
                mtmp = mx + moffx;
                mtmp = mtmp * mstepx;
                mtargetx = mpixelx + mtmp;
                mtmp = my + moffy;
                mtmp = mtmp * mstepy;
                mtargety = mpixely + mtmp;
                mtargetz = mpixelz;
                rcox = 0;
                rcoy = 0;
                rcoz = 0;
                rcdx = mtargetx;
                rcdy = mtargety;
                rcdz = mtargetz;
                rcdepth = mdepth;
                raycolor();
                msampler = rcoutr;
                msampleg = rcoutg;
                msampleb = rcoutb;
                mpixelr = mpixelr + msampler;
                mpixelg = mpixelg + msampleg;
                mpixelb = mpixelb + msampleb;
                msample = msample + 1;
                mcond = msample < msamples;
            }
            mpixelr = mpixelr / msamples;
            mpixelr = sqrt(mpixelr);
            mpixelg = mpixelg / msamples;
            mpixelg = sqrt(mpixelg);
            mpixelb = mpixelb / msamples;
            mpixelb = sqrt(mpixelb);
            mcond = mpixelr < 0;
            if (mcond) {
                mpixelr = 0;
            }
            mcond = mpixelr > 0.999;
            if (mcond) {
                mpixelr = 0.999;
            }
            mcond = mpixelg < 0;
            if (mcond) {
                mpixelg = 0;
            }
            mcond = mpixelg > 0.999;
            if (mcond) {
                mpixelg = 0.999;
            }
            mcond = mpixelb < 0;
            if (mcond) {
                mpixelb = 0;
            }
            mcond = mpixelb > 0.999;
            if (mcond) {
                mpixelb = 0.999;
            }
            moutr = mpixelr * 256;
            moutg = mpixelg * 256;
            moutb = mpixelb * 256;
            moutr = (int)moutr;
            moutg = (int)moutg;
            moutb = (int)moutb;
            printf("%d %d %d\n", (int)moutr, (int)moutg, (int)moutb);
            mx = mx + 1;
            mcond = mx < mwidth;
        }
        my = my + 1;
        mcond = my < mheight;
    }
    return 0;
}
