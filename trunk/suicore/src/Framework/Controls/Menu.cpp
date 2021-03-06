// ======================================================================
//
// Copyright (c) 2008-2022 汪荣(视点UI), Inc. All rights reserved.
//
// MPF界面库遵循指定开源协议，团体或公司商用请根据协议购买授权，
// 任何个人、团体或公司不能居于此库衍生任何商业性质的库或代码。
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// Menu.cpp

#include <Framework/Controls/Menu.h>
#include <Framework/Controls/MenuitemHelper.h>

#include <System/Windows/CoreTool.h>
#include <System/Input/MouseDevice.h>

namespace suic
{

ImplementRTTIOfClass(MenuBase, Selector)
ImplementRTTIOfClass(Menu, MenuBase)
ImplementRTTIOfClass(ContextMenu, MenuBase)

MenuBase::MenuBase()
    : _ownerMenu(NULL)
{
}

MenuBase::~MenuBase()
{
}

void MenuBase::Dispose()
{
    if (_currentSelection)
    {
        _currentSelection = MenuItemPtr();
    }

    Selector::Dispose();
}

MenuBase* MenuBase::GetOwner() const
{
    return _ownerMenu;
}

void MenuBase::SetOwner(MenuBase* owner)
{
    _ownerMenu = owner;
}

bool MenuBase::StaticInit()
{
    return true;
}

bool MenuBase::IsItemItsOwnContainer(Object* item)
{
    return (MenuItem::RTTIType() == item->GetRTTIType());
}

Element* MenuBase::GetContainerForItemOverride()
{
    return new MenuItem();
}

bool MenuBase::BlockRouteEvent() const
{
    return true;
}

void MenuBase::OnApplyTemplate()
{
    Selector::OnApplyTemplate();
}

void MenuBase::OnInitialized(EventArg* e)
{
    Selector::OnInitialized(e);
}

void MenuBase::OnRender(Drawing * drawing)
{
    Selector::OnRender(drawing);
}

void MenuBase::OnTextInput(KeyboardEventArg* e)
{
}

void MenuBase::OnKeyDown(KeyboardEventArg* e)
{
    Selector::OnKeyDown(e);
}

void MenuBase::OnMouseEnter(MouseButtonEventArg* e)
{
    Selector::OnMouseEnter(e);
    OnMouseMove(e);
}

void MenuBase::OnMouseMove(MouseButtonEventArg* e)
{
    Selector::OnMouseMove(e);

    if (MouseDevice::GetLeftButton() != MouseButtonState::mbMousePress)
    {
        return;
    }

    MenuItem* pOver = NULL;
    Panel* panel = GetItemsHost();
    ElementColl* elemColl = panel->GetChildren();

    for (int i = 0; i < panel->GetVisibleChildrenCount(); ++i)
    {
        MenuItem* pItem = dynamic_cast<MenuItem*>(elemColl->GetAt(i));

        if (pItem)
        {
            if (GetRenderBound(pItem).PointIn(e->GetMousePoint()))
            {
                pOver = pItem;

                break;
            }
        }
    }

    Element* pFocused = CASTTOE(GetFocusedItem());

    if (pOver != pFocused)
    {
        if (pFocused)
        {
            Selector::SelectContainer(pFocused, false);
        }

        if (pOver)
        {
            Selector::SelectContainer(pOver, true);
            pOver->Focus();
        }
    }

    e->SetHandled(true);
}

void MenuBase::OnMouseLeave(MouseButtonEventArg* e)
{
    Selector::OnMouseLeave(e);

    FrameworkElementPtr frame(GetFocusedItem());

    if (frame)
    {
        MouseButtonEventArg ef(NULL, e->GetMousePoint());
        ef.SetSource(frame.get());
        frame->OnMouseLeave(&ef);
    }
}

void MenuBase::OnMouseLeftButtonUp(MouseButtonEventArg* e)
{
    Selector::OnMouseLeftButtonUp(e);
    return;
}

bool Menu::StaticInit()
{
    return true;
}

Menu::Menu()
{
}

Menu::~Menu()
{
}

DpProperty* ContextMenu::IsOpenProperty;
DpProperty* ContextMenu::PlacementProperty;
DpProperty* ContextMenu::PlacementRectangleProperty;
DpProperty* ContextMenu::PlacementTargetProperty;
DpProperty* ContextMenu::VerticalOffsetProperty;
DpProperty* ContextMenu::HorizontalOffsetProperty;

void ContextMenu::OnIsOpenPropChanged(DpObject* d, DpPropChangedEventArg* e)
{
    ContextMenu* menu = RTTICast<ContextMenu>(d);
    if (e->GetNewValue()->ToBool())
    {
        if (menu->_parentPopup == NULL)
        {
            menu->HookupParentPopup();
        }
        TrackingMenuPopupHook::Ins()->TrackContextMenu(menu->_parentPopup, menu, 0, 0);
    }
    else
    {
        //menu->ClosingMenu();
    }
}

bool ContextMenu::StaticInit()
{
    if (NULL == IsOpenProperty)
    {
        Popup::StaticInit();

        IsOpenProperty = Popup::IsOpenProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(Boolean::False, 0, &ContextMenu::OnIsOpenPropChanged));
        PlacementProperty = Popup::PlacementProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(PlacementModeBox::MousePointBox, 0));
        PlacementRectangleProperty = Popup::PlacementRectangleProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(new ORect(), 0));
        PlacementTargetProperty = Popup::PlacementTargetProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(NULL, 0));
        VerticalOffsetProperty = Popup::VerticalOffsetProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetZeroInt(), 0));
        HorizontalOffsetProperty = Popup::HorizontalOffsetProperty->AddOwner(RTTIType(), DpPropMemory::GetPropMeta(Integer::GetZeroInt(), 0));
    }

    return true;
}

ContextMenu::ContextMenu()
    : _parentPopup(NULL)
{
}

ContextMenu::~ContextMenu()
{
    if (_parentPopup)
    {
        _parentPopup->unref();
    }
}

bool ContextMenu::IsOpen()
{
    return GetValue(IsOpenProperty)->ToBool();
}

void ContextMenu::SetIsOpen(bool val)
{
    SetValue(IsOpenProperty, BOOLTOBOOLEAN(val));
}

PlacementMode ContextMenu::GetPlacement()
{
    return (PlacementMode)GetValue(PlacementProperty)->ToInt();
}

void ContextMenu::SetPlacement(PlacementMode val)
{
    SetValue(PlacementProperty, new Integer(val));
}

Rect ContextMenu::GetPlacementRectangle()
{
    return ORect::FromObj(GetValue(PlacementRectangleProperty));
}

void ContextMenu::GetPlacementRectangle(Rect val)
{
    SetValue(PlacementRectangleProperty, new ORect(val));
}

Element* ContextMenu::GetPlacementTarget()
{
    return DynamicCast<Element>(GetValue(PlacementTargetProperty));
}

void ContextMenu::SetPlacementTarget(Element* target)
{
    SetValue(PlacementTargetProperty, target);
}

int ContextMenu::GetVerticalOffset()
{
    return GetValue(VerticalOffsetProperty)->ToInt();
}

void ContextMenu::SetVerticalOffset(int val)
{
    SetValue(VerticalOffsetProperty, new Integer(val));
}

int ContextMenu::GetHorizontalOffset()
{
    return GetValue(HorizontalOffsetProperty)->ToInt();
}

void ContextMenu::SetHorizontalOffset(int val)
{
    SetValue(HorizontalOffsetProperty, new Integer(val));
}

void ContextMenu::TrackContextMenu()
{
    SetIsOpen(true);
}

void ContextMenu::CloseContextMenu()
{
    GetMenuPopup()->CloseAllPopup(false);
}

void ContextMenu::OnPopupOpened(Object* sender, EventArg* e)
{
}

void ContextMenu::OnPopupClosed(Object* sender, EventArg* e)
{
}

Popup* ContextMenu::GetParentPopup()
{
    HookupParentPopup();
    return _parentPopup;
}

void ContextMenu::HookupParentPopup()
{
    if (_parentPopup == NULL)
    {
        _parentPopup = new Popup();
        _parentPopup->ref();

        _parentPopup->SetAllowsTransparency(true);
        _parentPopup->Opened += EventHandler(this, &ContextMenu::OnPopupOpened);
        _parentPopup->Closed += EventHandler(this, &ContextMenu::OnPopupClosed);
        Popup::CreateRootPopup(_parentPopup, this);
    }
}

}
