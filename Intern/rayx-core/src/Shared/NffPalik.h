#ifndef NFF_PALIK_H
#define NFF_PALIK_H

/** This struct represents one line of a .NKP file.  */
struct PalikEntry {
    double m_energy;
    double m_n;
    double m_k;
};

/** This struct represents one line of a .nff file.  */
struct NffEntry {
    double m_energy;
    double m_f1;
    double m_f2;
};

#endif
