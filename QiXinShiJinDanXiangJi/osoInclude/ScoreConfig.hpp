#pragma once

#include"oso_core.h"
#include <string>

namespace cdm {
    class ScoreConfig
    {
    public:
        ScoreConfig() = default;
        ~ScoreConfig() = default;

        ScoreConfig(const rw::oso::ObjectStoreAssembly& assembly);
        ScoreConfig(const ScoreConfig& obj);

        ScoreConfig& operator=(const ScoreConfig& obj);
        operator rw::oso::ObjectStoreAssembly() const;
        bool operator==(const ScoreConfig& obj) const;
        bool operator!=(const ScoreConfig& obj) const;

    public:
        bool Ng{ false };
        double NgScore{ 0 };
        double NgArea{ 0 };
    };

    inline ScoreConfig::ScoreConfig(const rw::oso::ObjectStoreAssembly& assembly)
    {
        auto isAccountAssembly = assembly.getName();
        if (isAccountAssembly != "$class$ScoreConfig$")
        {
            throw std::runtime_error("Assembly is not $class$ScoreConfig$");
        }
        auto NgItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$Ng$"));
        if (!NgItem) {
            throw std::runtime_error("$variable$Ng is not found");
        }
        Ng = NgItem->getValueAsBool();
        auto NgScoreItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$NgScore$"));
        if (!NgScoreItem) {
            throw std::runtime_error("$variable$NgScore is not found");
        }
        NgScore = NgScoreItem->getValueAsDouble();
        auto NgAreaItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$NgArea$"));
        if (!NgAreaItem) {
            throw std::runtime_error("$variable$NgArea is not found");
        }
        NgArea = NgAreaItem->getValueAsDouble();
    }

    inline ScoreConfig::ScoreConfig(const ScoreConfig& obj)
    {
        Ng = obj.Ng;
        NgScore = obj.NgScore;
        NgArea = obj.NgArea;
    }

    inline ScoreConfig& ScoreConfig::operator=(const ScoreConfig& obj)
    {
        if (this != &obj) {
            Ng = obj.Ng;
            NgScore = obj.NgScore;
            NgArea = obj.NgArea;
        }
        return *this;
    }

    inline ScoreConfig::operator rw::oso::ObjectStoreAssembly() const
    {
        rw::oso::ObjectStoreAssembly assembly;
        assembly.setName("$class$ScoreConfig$");
        auto NgItem = std::make_shared<rw::oso::ObjectStoreItem>();
        NgItem->setName("$variable$Ng$");
        NgItem->setValueFromBool(Ng);
        assembly.addItem(NgItem);
        auto NgScoreItem = std::make_shared<rw::oso::ObjectStoreItem>();
        NgScoreItem->setName("$variable$NgScore$");
        NgScoreItem->setValueFromDouble(NgScore);
        assembly.addItem(NgScoreItem);
        auto NgAreaItem = std::make_shared<rw::oso::ObjectStoreItem>();
        NgAreaItem->setName("$variable$NgArea$");
        NgAreaItem->setValueFromDouble(NgArea);
        assembly.addItem(NgAreaItem);
        return assembly;
    }

    inline bool ScoreConfig::operator==(const ScoreConfig& obj) const
    {
        return Ng == obj.Ng && NgScore == obj.NgScore && NgArea == obj.NgArea;
    }

    inline bool ScoreConfig::operator!=(const ScoreConfig& obj) const
    {
        return !(*this == obj);
    }

}

