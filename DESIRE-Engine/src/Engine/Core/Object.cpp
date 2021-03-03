#include "Engine/stdafx.h"
#include "Engine/Core/Object.h"

#include "Engine/Core/Component.h"
#include "Engine/Core/Math/Transform.h"

static constexpr uint32_t kMaxNumTransforms = 10000;
static Transform s_preallocatedTransforms[kMaxNumTransforms];
static size_t s_numTransforms = 0;

Object::Object()
{
	ASSERT(s_numTransforms < kMaxNumTransforms);
	m_pTransform = &s_preallocatedTransforms[s_numTransforms++];
	m_pTransform->m_pOwner = this;
	m_pTransform->ResetToIdentity();
}

Object::~Object()
{
	// If the owner of the transform is set to nullptr we are called from a parent object's destructor and no need to call SetParent()
	if(m_pTransform->m_pOwner)
	{
		SetParent(nullptr);
		s_numTransforms -= m_numTransformsInHierarchy;
	}

	for(Object* pChild : m_children)
	{
		pChild->m_pTransform->m_pOwner = nullptr;
		delete pChild;
	}

	m_pTransform->m_pParent = nullptr;
	m_pTransform->m_pOwner = nullptr;
}

void Object::SetObjectName(const String& name)
{
	m_objectName = name;
}

const String& Object::GetObjectName() const
{
	return m_objectName;
}

void Object::SetActive(bool active)
{
	m_isActive = active;
}

bool Object::IsActiveSelf() const
{
	return m_isActive;
}

bool Object::IsActiveInHierarchy() const
{
	const Object* pObj = this;
	while(pObj)
	{
		if(!pObj->IsActiveSelf())
		{
			return false;
		}

		pObj = pObj->GetParent();
	}

	return true;
}

void Object::SetParent(Object* pNewParent)
{
	if(m_pParent == pNewParent)
	{
		return;
	}

	if(m_pParent)
	{
		m_pParent->RemoveChild_Internal(this);
	}

	m_pParent = pNewParent;

	Transform* pOldTransform = m_pTransform;
	if(m_pParent)
	{
		m_pTransform = m_pParent->m_pTransform + m_pParent->m_numTransformsInHierarchy;

		// If the parent transform will be moved, we need to apply correction
		if(m_pParent->m_pTransform > pOldTransform)
		{
			m_pTransform -= m_numTransformsInHierarchy;
		}

		m_pParent->AddChild_Internal(this);
	}
	else
	{
		m_pTransform = &s_preallocatedTransforms[s_numTransforms];
	}

	ptrdiff_t numToMove = pOldTransform - m_pTransform;
	if(numToMove != 0)
	{
		Transform* pSavedTransforms = &s_preallocatedTransforms[s_numTransforms];
		ASSERT(m_numTransformsInHierarchy <= DESIRE_ASIZEOF(s_preallocatedTransforms) - s_numTransforms);
		memcpy(pSavedTransforms, pOldTransform, m_numTransformsInHierarchy * sizeof(Transform));

		Transform* pMovedTransformDst = nullptr;
		Transform* pMovedTransformSrc = nullptr;
		if(numToMove < 0)
		{
			numToMove = std::abs(numToMove);
			// Move data to the left in the array (our transforms will be placed after it)
			pMovedTransformDst = pOldTransform;
			pMovedTransformSrc = pOldTransform + m_numTransformsInHierarchy;
		}
		else
		{
			// Move data to the right in the array (our transforms will be placed before it)
			pMovedTransformDst = m_pTransform + m_numTransformsInHierarchy;
			pMovedTransformSrc = m_pTransform;
		}

		memmove(pMovedTransformDst, pMovedTransformSrc, numToMove * sizeof(Transform));
		RefreshParentPointerInTransforms(pMovedTransformDst, numToMove);

		memcpy(m_pTransform, pSavedTransforms, m_numTransformsInHierarchy * sizeof(Transform));
		RefreshParentPointerInTransforms(m_pTransform, m_numTransformsInHierarchy);
	}

	m_pTransform->m_flags |= Transform::WORLD_MATRIX_DIRTY;
	MarkAllChildrenTransformDirty();
}

Object* Object::GetParent() const
{
	return m_pParent;
}

Object& Object::CreateChildObject(const String& name)
{
	Object* pObject = new Object();
	pObject->SetObjectName(name);
	pObject->SetParent(this);
	return *pObject;
}

const Array<Object*>& Object::GetChildren() const
{
	return m_children;
}

Object* Object::FindObjectByName(const String& name, bool isRecursiveSearch) const
{
	for(Object* pChild : m_children)
	{
		if(name == pChild->GetObjectName())
		{
			return pChild;
		}
	}

	if(isRecursiveSearch)
	{
		for(Object* pChild : m_children)
		{
			Object* pFoundObject = pChild->FindObjectByName(name, isRecursiveSearch);
			if(pFoundObject)
			{
				return pFoundObject;
			}
		}
	}

	return nullptr;
}

bool Object::HasObjectInParentHierarchy(const Object* pObject) const
{
	const Object* pObj = GetParent();
	while(pObj)
	{
		if(pObj == pObject)
		{
			return true;
		}

		pObj = pObj->GetParent();
	}

	return false;
}

void Object::RemoveComponent(const Component* pComponent)
{
	const size_t idx = m_components.SpecializedFind([pComponent](const std::unique_ptr<Component>& spComponent)
	{
		return (spComponent.get() == pComponent);
	});

	if(idx != SIZE_MAX)
	{
		m_components.RemoveAt(idx);
	}
}

Component* Object::GetComponentByTypeId(int32_t typeId) const
{
	// Binary find
	auto iter = std::lower_bound(m_components.begin(), m_components.end(), typeId, [](const std::unique_ptr<Component>& spComponent, int32_t typeId)
	{
		return (spComponent->GetTypeId() < typeId);
	});
	return (iter != m_components.end() && (*iter)->GetTypeId() == typeId) ? iter->get() : nullptr;
}

const Array<std::unique_ptr<Component>>& Object::GetComponents() const
{
	return m_components;
}

Transform& Object::GetTransform() const
{
	return *m_pTransform;
}

void Object::MarkAllChildrenTransformDirty()
{
	Transform* pChildTransform = m_pTransform + 1;
	for(size_t i = 1; i < m_numTransformsInHierarchy; ++i)
	{
		pChildTransform->m_flags |= Transform::WORLD_MATRIX_DIRTY;
		pChildTransform++;
	}
}

Component& Object::AddComponent_Internal(std::unique_ptr<Component>&& spComponent)
{
	std::unique_ptr<Component>& spAddedComponent = m_components.BinaryFindOrInsert(std::move(spComponent), [](const std::unique_ptr<Component>& spLeft, const std::unique_ptr<Component>& spRight)
	{
		return (spLeft->GetTypeId() < spRight->GetTypeId());
	});
	return *spAddedComponent;
}

void Object::AddChild_Internal(Object* pChild)
{
	Object* pObj = this;
	do
	{
		pObj->m_numTransformsInHierarchy += pChild->m_numTransformsInHierarchy;
		pObj = pObj->GetParent();
	} while(pObj);

	m_children.Add(pChild);

	pChild->m_pTransform->m_pParent = m_pTransform;
}

void Object::RemoveChild_Internal(Object* pChild)
{
	Object* pObj = this;
	do
	{
		ASSERT(pObj->m_numTransformsInHierarchy > pChild->m_numTransformsInHierarchy);
		pObj->m_numTransformsInHierarchy -= pChild->m_numTransformsInHierarchy;
		pObj = pObj->GetParent();
	} while(pObj);

	m_children.Remove(pChild);
	pChild->m_pTransform->m_pParent = nullptr;
}

void Object::RefreshParentPointerInTransforms(Transform* pFirstTransform, size_t transformCount)
{
	Transform* pTransformTmp = pFirstTransform;
	for(size_t i = 0; i < transformCount; ++i)
	{
		pTransformTmp->m_pOwner->m_pTransform = pTransformTmp;
		if(pTransformTmp->m_pOwner->m_pParent)
		{
			pTransformTmp->m_pParent = &pTransformTmp->m_pOwner->m_pParent->GetTransform();
		}

		pTransformTmp++;
	}
}
