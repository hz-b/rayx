#pragma once

template <typename T>
struct DataSet {
    std::vector<T> values;
};

template <PhotonEnergy_c TPhotonEnergy>
struct RefractiveIndexDataSet {
    struct Item {
        TPhotonEnergy energyEv;
        double n;
        double k;
    };

    std::vector<Item> materialNames;
};

template <PhotonEnergy_c TPhotonEnergy>
struct MaterialRefractiveIndexDataSet {
    std::string materialName;
    RefractiveIndexDataSet<TPhotonEnergy> refractiveIndexDataSet;
};
