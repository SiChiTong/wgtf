//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  i_asset_browser_view_model.h
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef I_ASSET_BROWSER_VIEW_MODEL20_H_
#define I_ASSET_BROWSER_VIEW_MODEL20_H_

#pragma once

#include "core_reflection/reflected_object.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_variant/variant.hpp"

namespace wgt
{
class IListModel;
class ISelectionHandler;
class IValueChangeNotifier;

namespace AssetBrowser20
{
class IAssetObjectItem;
class IBreadcrumbsModel;
//------------------------------------------------------------------------------
// IAssetBrowserViewModel
//
// Represents the view model for the WGAssetBrowser control. Provides the
// data model used for the tree and list views as well as an event model.
// Provides properties used by the view based on the provided data model.
//------------------------------------------------------------------------------

class IAssetBrowserViewModel
{
public:
	IAssetBrowserViewModel()
	    : tempInt_(0)
	{
	}

	virtual ~IAssetBrowserViewModel()
	{
	}

	// Retrieve the view model
	virtual const IAssetBrowserViewModel* view() const
	{
		return this;
	}

	// Retrieve the data model
	// Expected: IAssetBrowserModel
	virtual ObjectHandle data() const
	{
		return ObjectHandle();
	}

	// Retrieve the event model
	// Expected: IAssetBrowserEventModel
	virtual ObjectHandle events() const
	{
		return ObjectHandle();
	}

	// Retrieve the breadcrumbs model
	virtual IBreadcrumbsModel* getBreadcrumbsModel() const
	{
		return nullptr;
	}

	virtual void onItemSelected(std::string assetPath)
	{
	}

	// Asset usage handlers (note: pattern likely to change in future iterations)
	virtual bool useSelectedAsset() const
	{
		return true;
	}
	virtual const int& currentSelectedAssetIndex() const
	{
		return tempInt_;
	}
	virtual void currentSelectedAssetIndex(const int& index)
	{
	}

	// Retrieve the selected asset data - not exposed to QML. For native-use only.
	virtual IAssetObjectItem* getSelectedAssetData() const
	{
		return nullptr;
	}

	// Invokes a refresh of the data models based on plugin states. How the refresh is handled is
	// entirely up to the developer.
	// Expected: Boolean
	virtual bool refreshData() const
	{
		return true;
	}

private:
	int tempInt_;
};
} // end namespace AssetBrowser20

} // end namespace wgt
#endif // I_ASSET_BROWSER_VIEW_MODEL20_H_
