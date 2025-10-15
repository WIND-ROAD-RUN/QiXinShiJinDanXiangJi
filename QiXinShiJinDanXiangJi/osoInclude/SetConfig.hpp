#pragma once

#include"oso_core.h"
#include <string>

namespace cdm {
    class SetConfig
    {
    public:
        SetConfig() = default;
        ~SetConfig() = default;

        SetConfig(const rw::oso::ObjectStoreAssembly& assembly);
        SetConfig(const SetConfig& obj);

        SetConfig& operator=(const SetConfig& obj);
        operator rw::oso::ObjectStoreAssembly() const;
        bool operator==(const SetConfig& obj) const;
        bool operator!=(const SetConfig& obj) const;

    public:
        double tiFeiChiXuShiJian1{ 0 };
        double tifeijuli1{ 0 };
        double tiFeiChiXuShiJian2{ 0 };
        double tifeijuli2{ 0 };
        double shangXianWei1{ 0 };
        double xiaXianWei1{ 0 };
        double zuoXianWei1{ 0 };
        double youXianWei1{ 0 };
        double baoguang1{ 0 };
        double zengyi1{ 0 };
        double xiangSuDangLiang1{ 0 };
        double shangXianWei2{ 0 };
        double xiaXianWei2{ 0 };
        double zuoXianWei2{ 0 };
        double youXianWei2{ 0 };
        double baoguang2{ 0 };
        double zengyi2{ 0 };
        double xiangSuDangLiang2{ 0 };
        bool takeWork1Pictures{ false };
        bool takeWork2Pictures{ false };
        bool saveNGImg{ false };
        bool saveMaskImg{ false };
        bool saveOKImg{ false };
        bool qiyongerxiangji{ false };
        bool qiyongxiangjichonglian{ false };
    };

    inline SetConfig::SetConfig(const rw::oso::ObjectStoreAssembly& assembly)
    {
        auto isAccountAssembly = assembly.getName();
        if (isAccountAssembly != "$class$SetConfig$")
        {
            throw std::runtime_error("Assembly is not $class$SetConfig$");
        }
        auto tiFeiChiXuShiJian1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$tiFeiChiXuShiJian1$"));
        if (!tiFeiChiXuShiJian1Item) {
            throw std::runtime_error("$variable$tiFeiChiXuShiJian1 is not found");
        }
        tiFeiChiXuShiJian1 = tiFeiChiXuShiJian1Item->getValueAsDouble();
        auto tifeijuli1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$tifeijuli1$"));
        if (!tifeijuli1Item) {
            throw std::runtime_error("$variable$tifeijuli1 is not found");
        }
        tifeijuli1 = tifeijuli1Item->getValueAsDouble();
        auto tiFeiChiXuShiJian2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$tiFeiChiXuShiJian2$"));
        if (!tiFeiChiXuShiJian2Item) {
            throw std::runtime_error("$variable$tiFeiChiXuShiJian2 is not found");
        }
        tiFeiChiXuShiJian2 = tiFeiChiXuShiJian2Item->getValueAsDouble();
        auto tifeijuli2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$tifeijuli2$"));
        if (!tifeijuli2Item) {
            throw std::runtime_error("$variable$tifeijuli2 is not found");
        }
        tifeijuli2 = tifeijuli2Item->getValueAsDouble();
        auto shangXianWei1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$shangXianWei1$"));
        if (!shangXianWei1Item) {
            throw std::runtime_error("$variable$shangXianWei1 is not found");
        }
        shangXianWei1 = shangXianWei1Item->getValueAsDouble();
        auto xiaXianWei1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$xiaXianWei1$"));
        if (!xiaXianWei1Item) {
            throw std::runtime_error("$variable$xiaXianWei1 is not found");
        }
        xiaXianWei1 = xiaXianWei1Item->getValueAsDouble();
        auto zuoXianWei1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$zuoXianWei1$"));
        if (!zuoXianWei1Item) {
            throw std::runtime_error("$variable$zuoXianWei1 is not found");
        }
        zuoXianWei1 = zuoXianWei1Item->getValueAsDouble();
        auto youXianWei1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$youXianWei1$"));
        if (!youXianWei1Item) {
            throw std::runtime_error("$variable$youXianWei1 is not found");
        }
        youXianWei1 = youXianWei1Item->getValueAsDouble();
        auto baoguang1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$baoguang1$"));
        if (!baoguang1Item) {
            throw std::runtime_error("$variable$baoguang1 is not found");
        }
        baoguang1 = baoguang1Item->getValueAsDouble();
        auto zengyi1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$zengyi1$"));
        if (!zengyi1Item) {
            throw std::runtime_error("$variable$zengyi1 is not found");
        }
        zengyi1 = zengyi1Item->getValueAsDouble();
        auto xiangSuDangLiang1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$xiangSuDangLiang1$"));
        if (!xiangSuDangLiang1Item) {
            throw std::runtime_error("$variable$xiangSuDangLiang1 is not found");
        }
        xiangSuDangLiang1 = xiangSuDangLiang1Item->getValueAsDouble();
        auto shangXianWei2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$shangXianWei2$"));
        if (!shangXianWei2Item) {
            throw std::runtime_error("$variable$shangXianWei2 is not found");
        }
        shangXianWei2 = shangXianWei2Item->getValueAsDouble();
        auto xiaXianWei2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$xiaXianWei2$"));
        if (!xiaXianWei2Item) {
            throw std::runtime_error("$variable$xiaXianWei2 is not found");
        }
        xiaXianWei2 = xiaXianWei2Item->getValueAsDouble();
        auto zuoXianWei2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$zuoXianWei2$"));
        if (!zuoXianWei2Item) {
            throw std::runtime_error("$variable$zuoXianWei2 is not found");
        }
        zuoXianWei2 = zuoXianWei2Item->getValueAsDouble();
        auto youXianWei2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$youXianWei2$"));
        if (!youXianWei2Item) {
            throw std::runtime_error("$variable$youXianWei2 is not found");
        }
        youXianWei2 = youXianWei2Item->getValueAsDouble();
        auto baoguang2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$baoguang2$"));
        if (!baoguang2Item) {
            throw std::runtime_error("$variable$baoguang2 is not found");
        }
        baoguang2 = baoguang2Item->getValueAsDouble();
        auto zengyi2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$zengyi2$"));
        if (!zengyi2Item) {
            throw std::runtime_error("$variable$zengyi2 is not found");
        }
        zengyi2 = zengyi2Item->getValueAsDouble();
        auto xiangSuDangLiang2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$xiangSuDangLiang2$"));
        if (!xiangSuDangLiang2Item) {
            throw std::runtime_error("$variable$xiangSuDangLiang2 is not found");
        }
        xiangSuDangLiang2 = xiangSuDangLiang2Item->getValueAsDouble();
        auto takeWork1PicturesItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$takeWork1Pictures$"));
        if (!takeWork1PicturesItem) {
            throw std::runtime_error("$variable$takeWork1Pictures is not found");
        }
        takeWork1Pictures = takeWork1PicturesItem->getValueAsBool();
        auto takeWork2PicturesItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$takeWork2Pictures$"));
        if (!takeWork2PicturesItem) {
            throw std::runtime_error("$variable$takeWork2Pictures is not found");
        }
        takeWork2Pictures = takeWork2PicturesItem->getValueAsBool();
        auto saveNGImgItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$saveNGImg$"));
        if (!saveNGImgItem) {
            throw std::runtime_error("$variable$saveNGImg is not found");
        }
        saveNGImg = saveNGImgItem->getValueAsBool();
        auto saveMaskImgItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$saveMaskImg$"));
        if (!saveMaskImgItem) {
            throw std::runtime_error("$variable$saveMaskImg is not found");
        }
        saveMaskImg = saveMaskImgItem->getValueAsBool();
        auto saveOKImgItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$saveOKImg$"));
        if (!saveOKImgItem) {
            throw std::runtime_error("$variable$saveOKImg is not found");
        }
        saveOKImg = saveOKImgItem->getValueAsBool();
        auto qiyongerxiangjiItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$qiyongerxiangji$"));
        if (!qiyongerxiangjiItem) {
            throw std::runtime_error("$variable$qiyongerxiangji is not found");
        }
        qiyongerxiangji = qiyongerxiangjiItem->getValueAsBool();
        auto qiyongxiangjichonglianItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$qiyongxiangjichonglian$"));
        if (!qiyongxiangjichonglianItem) {
            throw std::runtime_error("$variable$qiyongxiangjichonglian is not found");
        }
        qiyongxiangjichonglian = qiyongxiangjichonglianItem->getValueAsBool();
    }

    inline SetConfig::SetConfig(const SetConfig& obj)
    {
        tiFeiChiXuShiJian1 = obj.tiFeiChiXuShiJian1;
        tifeijuli1 = obj.tifeijuli1;
        tiFeiChiXuShiJian2 = obj.tiFeiChiXuShiJian2;
        tifeijuli2 = obj.tifeijuli2;
        shangXianWei1 = obj.shangXianWei1;
        xiaXianWei1 = obj.xiaXianWei1;
        zuoXianWei1 = obj.zuoXianWei1;
        youXianWei1 = obj.youXianWei1;
        baoguang1 = obj.baoguang1;
        zengyi1 = obj.zengyi1;
        xiangSuDangLiang1 = obj.xiangSuDangLiang1;
        shangXianWei2 = obj.shangXianWei2;
        xiaXianWei2 = obj.xiaXianWei2;
        zuoXianWei2 = obj.zuoXianWei2;
        youXianWei2 = obj.youXianWei2;
        baoguang2 = obj.baoguang2;
        zengyi2 = obj.zengyi2;
        xiangSuDangLiang2 = obj.xiangSuDangLiang2;
        takeWork1Pictures = obj.takeWork1Pictures;
        takeWork2Pictures = obj.takeWork2Pictures;
        saveNGImg = obj.saveNGImg;
        saveMaskImg = obj.saveMaskImg;
        saveOKImg = obj.saveOKImg;
        qiyongerxiangji = obj.qiyongerxiangji;
        qiyongxiangjichonglian = obj.qiyongxiangjichonglian;
    }

    inline SetConfig& SetConfig::operator=(const SetConfig& obj)
    {
        if (this != &obj) {
            tiFeiChiXuShiJian1 = obj.tiFeiChiXuShiJian1;
            tifeijuli1 = obj.tifeijuli1;
            tiFeiChiXuShiJian2 = obj.tiFeiChiXuShiJian2;
            tifeijuli2 = obj.tifeijuli2;
            shangXianWei1 = obj.shangXianWei1;
            xiaXianWei1 = obj.xiaXianWei1;
            zuoXianWei1 = obj.zuoXianWei1;
            youXianWei1 = obj.youXianWei1;
            baoguang1 = obj.baoguang1;
            zengyi1 = obj.zengyi1;
            xiangSuDangLiang1 = obj.xiangSuDangLiang1;
            shangXianWei2 = obj.shangXianWei2;
            xiaXianWei2 = obj.xiaXianWei2;
            zuoXianWei2 = obj.zuoXianWei2;
            youXianWei2 = obj.youXianWei2;
            baoguang2 = obj.baoguang2;
            zengyi2 = obj.zengyi2;
            xiangSuDangLiang2 = obj.xiangSuDangLiang2;
            takeWork1Pictures = obj.takeWork1Pictures;
            takeWork2Pictures = obj.takeWork2Pictures;
            saveNGImg = obj.saveNGImg;
            saveMaskImg = obj.saveMaskImg;
            saveOKImg = obj.saveOKImg;
            qiyongerxiangji = obj.qiyongerxiangji;
            qiyongxiangjichonglian = obj.qiyongxiangjichonglian;
        }
        return *this;
    }

    inline SetConfig::operator rw::oso::ObjectStoreAssembly() const
    {
        rw::oso::ObjectStoreAssembly assembly;
        assembly.setName("$class$SetConfig$");
        auto tiFeiChiXuShiJian1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        tiFeiChiXuShiJian1Item->setName("$variable$tiFeiChiXuShiJian1$");
        tiFeiChiXuShiJian1Item->setValueFromDouble(tiFeiChiXuShiJian1);
        assembly.addItem(tiFeiChiXuShiJian1Item);
        auto tifeijuli1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        tifeijuli1Item->setName("$variable$tifeijuli1$");
        tifeijuli1Item->setValueFromDouble(tifeijuli1);
        assembly.addItem(tifeijuli1Item);
        auto tiFeiChiXuShiJian2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        tiFeiChiXuShiJian2Item->setName("$variable$tiFeiChiXuShiJian2$");
        tiFeiChiXuShiJian2Item->setValueFromDouble(tiFeiChiXuShiJian2);
        assembly.addItem(tiFeiChiXuShiJian2Item);
        auto tifeijuli2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        tifeijuli2Item->setName("$variable$tifeijuli2$");
        tifeijuli2Item->setValueFromDouble(tifeijuli2);
        assembly.addItem(tifeijuli2Item);
        auto shangXianWei1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        shangXianWei1Item->setName("$variable$shangXianWei1$");
        shangXianWei1Item->setValueFromDouble(shangXianWei1);
        assembly.addItem(shangXianWei1Item);
        auto xiaXianWei1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        xiaXianWei1Item->setName("$variable$xiaXianWei1$");
        xiaXianWei1Item->setValueFromDouble(xiaXianWei1);
        assembly.addItem(xiaXianWei1Item);
        auto zuoXianWei1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        zuoXianWei1Item->setName("$variable$zuoXianWei1$");
        zuoXianWei1Item->setValueFromDouble(zuoXianWei1);
        assembly.addItem(zuoXianWei1Item);
        auto youXianWei1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        youXianWei1Item->setName("$variable$youXianWei1$");
        youXianWei1Item->setValueFromDouble(youXianWei1);
        assembly.addItem(youXianWei1Item);
        auto baoguang1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        baoguang1Item->setName("$variable$baoguang1$");
        baoguang1Item->setValueFromDouble(baoguang1);
        assembly.addItem(baoguang1Item);
        auto zengyi1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        zengyi1Item->setName("$variable$zengyi1$");
        zengyi1Item->setValueFromDouble(zengyi1);
        assembly.addItem(zengyi1Item);
        auto xiangSuDangLiang1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        xiangSuDangLiang1Item->setName("$variable$xiangSuDangLiang1$");
        xiangSuDangLiang1Item->setValueFromDouble(xiangSuDangLiang1);
        assembly.addItem(xiangSuDangLiang1Item);
        auto shangXianWei2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        shangXianWei2Item->setName("$variable$shangXianWei2$");
        shangXianWei2Item->setValueFromDouble(shangXianWei2);
        assembly.addItem(shangXianWei2Item);
        auto xiaXianWei2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        xiaXianWei2Item->setName("$variable$xiaXianWei2$");
        xiaXianWei2Item->setValueFromDouble(xiaXianWei2);
        assembly.addItem(xiaXianWei2Item);
        auto zuoXianWei2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        zuoXianWei2Item->setName("$variable$zuoXianWei2$");
        zuoXianWei2Item->setValueFromDouble(zuoXianWei2);
        assembly.addItem(zuoXianWei2Item);
        auto youXianWei2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        youXianWei2Item->setName("$variable$youXianWei2$");
        youXianWei2Item->setValueFromDouble(youXianWei2);
        assembly.addItem(youXianWei2Item);
        auto baoguang2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        baoguang2Item->setName("$variable$baoguang2$");
        baoguang2Item->setValueFromDouble(baoguang2);
        assembly.addItem(baoguang2Item);
        auto zengyi2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        zengyi2Item->setName("$variable$zengyi2$");
        zengyi2Item->setValueFromDouble(zengyi2);
        assembly.addItem(zengyi2Item);
        auto xiangSuDangLiang2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        xiangSuDangLiang2Item->setName("$variable$xiangSuDangLiang2$");
        xiangSuDangLiang2Item->setValueFromDouble(xiangSuDangLiang2);
        assembly.addItem(xiangSuDangLiang2Item);
        auto takeWork1PicturesItem = std::make_shared<rw::oso::ObjectStoreItem>();
        takeWork1PicturesItem->setName("$variable$takeWork1Pictures$");
        takeWork1PicturesItem->setValueFromBool(takeWork1Pictures);
        assembly.addItem(takeWork1PicturesItem);
        auto takeWork2PicturesItem = std::make_shared<rw::oso::ObjectStoreItem>();
        takeWork2PicturesItem->setName("$variable$takeWork2Pictures$");
        takeWork2PicturesItem->setValueFromBool(takeWork2Pictures);
        assembly.addItem(takeWork2PicturesItem);
        auto saveNGImgItem = std::make_shared<rw::oso::ObjectStoreItem>();
        saveNGImgItem->setName("$variable$saveNGImg$");
        saveNGImgItem->setValueFromBool(saveNGImg);
        assembly.addItem(saveNGImgItem);
        auto saveMaskImgItem = std::make_shared<rw::oso::ObjectStoreItem>();
        saveMaskImgItem->setName("$variable$saveMaskImg$");
        saveMaskImgItem->setValueFromBool(saveMaskImg);
        assembly.addItem(saveMaskImgItem);
        auto saveOKImgItem = std::make_shared<rw::oso::ObjectStoreItem>();
        saveOKImgItem->setName("$variable$saveOKImg$");
        saveOKImgItem->setValueFromBool(saveOKImg);
        assembly.addItem(saveOKImgItem);
        auto qiyongerxiangjiItem = std::make_shared<rw::oso::ObjectStoreItem>();
        qiyongerxiangjiItem->setName("$variable$qiyongerxiangji$");
        qiyongerxiangjiItem->setValueFromBool(qiyongerxiangji);
        assembly.addItem(qiyongerxiangjiItem);
        auto qiyongxiangjichonglianItem = std::make_shared<rw::oso::ObjectStoreItem>();
        qiyongxiangjichonglianItem->setName("$variable$qiyongxiangjichonglian$");
        qiyongxiangjichonglianItem->setValueFromBool(qiyongxiangjichonglian);
        assembly.addItem(qiyongxiangjichonglianItem);
        return assembly;
    }

    inline bool SetConfig::operator==(const SetConfig& obj) const
    {
        return tiFeiChiXuShiJian1 == obj.tiFeiChiXuShiJian1 && tifeijuli1 == obj.tifeijuli1 && tiFeiChiXuShiJian2 == obj.tiFeiChiXuShiJian2 && tifeijuli2 == obj.tifeijuli2 && shangXianWei1 == obj.shangXianWei1 && xiaXianWei1 == obj.xiaXianWei1 && zuoXianWei1 == obj.zuoXianWei1 && youXianWei1 == obj.youXianWei1 && baoguang1 == obj.baoguang1 && zengyi1 == obj.zengyi1 && xiangSuDangLiang1 == obj.xiangSuDangLiang1 && shangXianWei2 == obj.shangXianWei2 && xiaXianWei2 == obj.xiaXianWei2 && zuoXianWei2 == obj.zuoXianWei2 && youXianWei2 == obj.youXianWei2 && baoguang2 == obj.baoguang2 && zengyi2 == obj.zengyi2 && xiangSuDangLiang2 == obj.xiangSuDangLiang2 && takeWork1Pictures == obj.takeWork1Pictures && takeWork2Pictures == obj.takeWork2Pictures && saveNGImg == obj.saveNGImg && saveMaskImg == obj.saveMaskImg && saveOKImg == obj.saveOKImg && qiyongerxiangji == obj.qiyongerxiangji && qiyongxiangjichonglian == obj.qiyongxiangjichonglian;
    }

    inline bool SetConfig::operator!=(const SetConfig& obj) const
    {
        return !(*this == obj);
    }

}

