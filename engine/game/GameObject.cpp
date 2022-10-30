#include "GameObject.h"

using namespace engine;
using namespace engine::game;

static uint64_t _objectIdCounter{ 0 };

CGameObject::CGameObject()
{
    objectId = _objectIdCounter++;
}

CGameObject::CGameObject(const std::string& srName)
{
    this->srName = srName;
}

CGameObject::~CGameObject()
{
    mChilds.clear();
}

void CGameObject::create()
{
    for (auto& [name, child] : mChilds)
        child->create();
}

void CGameObject::reCreate()
{
    for (auto& [name, child] : mChilds)
        child->reCreate();
}

void CGameObject::render()
{
    for (auto& [name, child] : mChilds)
    {
        if (child->isVisible())
            child->render();
    }
}

void CGameObject::update(float fDeltaTime)
{
    transform_old = transform;
    for (auto& [name, child] : mChilds)
    {
        if (child->isEnabled())
            child->update(fDeltaTime);
    }
}

std::shared_ptr<CGameObject>& CGameObject::find(const std::string& name)
{
    auto it = mChilds.find(srName);
    if (it != mChilds.end())
        return it->second;

    for (auto& [name, child] : mChilds)
        return child->find(srName);
    return pNull;
}

std::shared_ptr<CGameObject>& CGameObject::find(uint64_t id)
{
    for (auto& [name, child] : mChilds)
    {
        if (child->objectId == id)
            return child;
        return child->find(id);
    }
    return pNull;
}

void CGameObject::addChild(std::shared_ptr<CGameObject>& child)
{
    mChilds.emplace(child->srName, child);
    child->setParent(shared_from_this());
}

void CGameObject::setParent(std::shared_ptr<CGameObject> parent)
{
    pParentOld = pParent;
    pParent = parent;
    // If you set parent for this, you should attach self to parent's child's
    if (pParentOld)
        pParentOld->detach(shared_from_this());
}

void CGameObject::attach(std::shared_ptr<CGameObject>&& child)
{
    addChild(child);
}

void CGameObject::detach(std::shared_ptr<CGameObject> child)
{
    auto it = mChilds.find(child->srName);
    if (it != mChilds.end())
        mChilds.erase(it);
}

std::shared_ptr<CGameObject>& CGameObject::getParent()
{
    return pParent;
}

std::unordered_map<std::string, std::shared_ptr<CGameObject>>& CGameObject::getChilds()
{
    return mChilds;
}

uint64_t CGameObject::getId() const
{ 
    return objectId; 
}

void CGameObject::setName(const std::string& name)
{
    if (name == srName)
        return;

    if (pParent)
    {
        auto& childs = pParent->getChilds();
        auto child = childs.extract(srName);
        if (child)
        {
            child.key() = name;
            childs.insert(std::move(child));
        }
    }
    srName = name;
}

const std::string& CGameObject::getName()
{
    return srName;
}

void CGameObject::setTransform(FTransform transformNew)
{
    transform = transformNew;
}

FTransform CGameObject::getTransform()
{
    auto t = transform;
    if (pParent)
        t += pParent->getTransform();
    return t;
}

FTransform& CGameObject::getLocalTransform() 
{ 
    return transform; 
}

glm::mat4 CGameObject::getModel()
{
    if (pParent)
        return pParent->getModel() * transform.getModel();
    return transform.getModel();
}

glm::mat4 CGameObject::getModelOld()
{
    if (pParent)
        return pParent->getModelOld() * transform_old.getModel();
    return transform_old.getModel();
}

void CGameObject::setPosition(const glm::vec3& position)
{
    transform.setPosition(position);
}

glm::vec3 CGameObject::getPosition()
{
    FTransform global = this->getTransform();
    return global.getPosition();
}

const glm::vec3 CGameObject::getLocalPosition() const 
{ 
    return transform.getPosition(); 
}

void CGameObject::setRotation(const glm::vec3& rotation)
{
    transform.setRotation(rotation);
}

glm::vec3 CGameObject::getRotation()
{
    auto global = this->getTransform();
    return global.getRotation();
}

const glm::vec3 CGameObject::getLocalRotation() const 
{ 
    return transform.getRotation(); 
}

void CGameObject::setScale(const glm::vec3& scale)
{
    transform.setScale(scale);
}

glm::vec3 CGameObject::getScale()
{
    auto global = this->getTransform();
    return global.getScale();
}

const glm::vec3 CGameObject::getLocalScale() const 
{ 
    return transform.getScale(); 
}

void CGameObject::setIndex(uint32_t index) 
{ 
    objectIndex = index; 
}

uint32_t CGameObject::getIndex() 
{ 
    return objectIndex; 
}

void CGameObject::setVisible(bool value) 
{ 
    bVisible = value; 
}

bool CGameObject::isVisible() const 
{ 
    return bVisible; 
}

void CGameObject::setEnable(bool value) 
{ 
    bEnable = value; 
}

bool CGameObject::isEnabled() const 
{ 
    return bEnable; 
}
