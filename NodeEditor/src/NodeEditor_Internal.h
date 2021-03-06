﻿# pragma once
# define IMGUI_DEFINE_MATH_OPERATORS
# include "NodeEditor.h"
# include "imgui_internal.h"
# include "imguiex_internal.h"
# include "imguiex_canvas.h"
# include "Actions.h"
# include "Utilities/BitmaskType.h"

namespace ax {
namespace NodeEditor {

static const ImU32  c_ConfigBackgroundColor     = ImColor( 60,  60,  70, 200);
static const float  c_ConfigGridSize            = 50.0f;//32.0f;
static const ImU32  c_ConfigGridColor           = ImColor(120, 120, 120,  40);
static const int    c_ConfigDragNodeButtonIndex = 0;
static const int    c_ConfigSelectButtonIndex   = 0;
static const int    c_ConfigScrollButtonIndex   = 1;

struct Object;
struct Pin;
struct Node;
struct Link;
struct Canvas;

struct Action;
struct NavigateAction;

struct Editor;
struct NodeBuilder;

struct CanvasId final: SafePointerType<CanvasId> { using SafePointerType::SafePointerType; };

enum class ObjectType : uint8_t
{
    Pin,
    Node,
    Link,
    Canvas
};

enum class ObjectTypes : uint8_t
{
    None      = 0,
    Pins      = 1 << static_cast<uint8_t>(ObjectType::Pin),
    Nodes     = 1 << static_cast<uint8_t>(ObjectType::Node),
    Links     = 1 << static_cast<uint8_t>(ObjectType::Link),
    Canvases  = 1 << static_cast<uint8_t>(ObjectType::Canvas),
    All       = Pins | Nodes | Links | Canvases
};
AX_BITMASK_TYPE(ObjectTypes);

struct ObjectId final: SafePointerType<ObjectId>
{
    using SafePointerType::SafePointerType;
    ObjectId(PinId  pinId):      SafePointerType(pinId.AsPointer()),    m_Type(ObjectType::Pin)    {}
    ObjectId(NodeId nodeId):     SafePointerType(nodeId.AsPointer()),   m_Type(ObjectType::Node)   {}
    ObjectId(LinkId linkId):     SafePointerType(linkId.AsPointer()),   m_Type(ObjectType::Link)   {}
    ObjectId(CanvasId canvasId): SafePointerType(canvasId.AsPointer()), m_Type(ObjectType::Canvas) {}

    explicit operator PinId()    const { return AsPinId();    }
    explicit operator NodeId()   const { return AsNodeId();   }
    explicit operator LinkId()   const { return AsLinkId();   }
    explicit operator CanvasId() const { return AsCanvasId(); }

    PinId    AsPinId()    const { IM_ASSERT(IsPinId());    return PinId(AsPointer());    }
    NodeId   AsNodeId()   const { IM_ASSERT(IsNodeId());   return NodeId(AsPointer());   }
    LinkId   AsLinkId()   const { IM_ASSERT(IsLinkId());   return LinkId(AsPointer());   }
    CanvasId AsCanvasId() const { IM_ASSERT(IsCanvasId()); return CanvasId(AsPointer()); }

    bool IsPinId()    const { return m_Type == ObjectType::Pin;    }
    bool IsNodeId()   const { return m_Type == ObjectType::Node;   }
    bool IsLinkId()   const { return m_Type == ObjectType::Link;   }
    bool IsCanvasId() const { return m_Type == ObjectType::Canvas; }

    ObjectType Type() const { return m_Type; }

private:
    ObjectType m_Type;
};

struct Object
{
    Object(ObjectId id);

    ObjectId m_Id;
    bool     m_IsLive = false;

    ObjectType Type() const;

    void Touch();
    void Reset();

    Pin*  AsPin();
    const Pin*  AsPin()  const;
    Node* AsNode();
    const Node* AsNode() const;
    Link* AsLink();
    const Link* AsLink() const;
};

// #todo: Make collection sorted to speed up Find()
template <typename T>
struct ObjectCollection
{
    ~ObjectCollection();

    T* Create(ObjectId id);
    T* Find(ObjectId id);
    const T* Find(ObjectId id) const;
    T* Get(ObjectId id);

    int Count() const;

    void Reset();

    const ImVector<T*>&       View()       { return m_Objects; }
    const ImVector<const T*>& View() const { return m_Objects; }

private:
    ImVector<T*> m_Objects;
};

struct Pin final : Object
{
    using Object::Object;
};

struct Node final : Object
{
    using Object::Object;

    ImRect m_Bounds = ImRect(0, 0, 0, 0);
};

struct Link final : Object
{
    using Object::Object;
};

struct Canvas final : Object
{
    using Object::Object;
};

enum class KeyModifers : uint8_t
{
    None    = 0,
    Ctrl    = 1,
    Shift   = 2,
    Alt     = 4,
    Super   = 8
};
AX_BITMASK_TYPE(KeyModifers);

template <typename T>
struct InputItem
{
    T Hovered       = T{};
    T Active        = T{};
    T Clicked       = T{};
    T DoubleClicked = T{};
};

struct InputState
{
    ImVec2             MousePosition;
    InputItem<Object*> Object;
    InputItem<Pin*>    Pin;
    InputItem<Node*>   Node;
    InputItem<Link*>   Link;
    InputItem<Canvas*> Canvas;
    KeyModifers        Modifiers = KeyModifers::None;
};

enum class SelectOperation
{
    Replace,
    Add,
    Remove,
    Toggle
};

struct Selection
{
    bool IsSelected(Object* object) const;

    bool Select(Object* object, SelectOperation operation = SelectOperation::Replace);
    bool Select(const ImVector<Object*>& objects, SelectOperation operation = SelectOperation::Replace);

    void DeselectAll();

    ImVector<Object*> Objects;
};

namespace Debug {
const char*     ToString(ObjectType type);
ImGuiTextBuffer ToString(const Object* o);
ImGuiTextBuffer ToString(const ImVec2& p);
ImGuiTextBuffer ToString(const ImRect& rect);
ImGuiTextBuffer ToString(const ImGuiEx::CanvasView& view);
ImGuiTextBuffer ToString(KeyModifers modifiers);
} // namespace Debug

} // namespace NodeEditor
} // namespace ax


# include "NodeEditor_Internal.inl"