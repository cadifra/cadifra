/*
 *     Copyright (c) 2025 Adrian & Frank Buehlmann. ALL RIGHTS RESERVED.
 */

export module Editor.TransferSetFormatHandler;

import App.Com;
import App.IApplication;

import Store.IStorable;

import View.Base;


namespace Editor
{

export class TransferSetFormatHandler:
    public View::ITransferSetFormatHandler
{
    const Store::FactoryRegistry& factoryRegistry_;
    App::IApplication& application_;

public:
    //-- View::ITransferSetFormatHandler

    void addFormatProvider(
        App::DataProvider&, Core::TransferSet*) const override;

    void addFormatProvider(
        App::DataProvider& dp, View::ITransferSetCreator& c) const override;

    bool QueryExtract(App::IDataHolder& dh) const override;

    void extract(App::IDataHolder& dh, ITransferSetUser& u) const override;

    //--

    TransferSetFormatHandler(const Store::FactoryRegistry&, App::IApplication&);
};

}
