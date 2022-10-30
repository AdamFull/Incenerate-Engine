#pragma once

#include "Transform.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace engine
{
	namespace game
	{
		class CGameObject : public std::enable_shared_from_this<CGameObject>
		{
		public:
            CGameObject();
            CGameObject(const std::string& srName);
            virtual ~CGameObject();

            virtual void create();
            virtual void reCreate();
            virtual void render();
            virtual void update(float fDeltaTime);

            std::shared_ptr<CGameObject>& find(const std::string& name);
            std::shared_ptr<CGameObject>& find(uint64_t id);
            void addChild(std::shared_ptr<CGameObject>& child);
            void setParent(std::shared_ptr<CGameObject> parent);
            void attach(std::shared_ptr<CGameObject>&& child);
            void detach(std::shared_ptr<CGameObject> child);
            std::shared_ptr<CGameObject>& getParent();
            std::unordered_map<std::string, std::shared_ptr<CGameObject>>& getChilds();

            uint64_t getId() const;

            void setName(const std::string& name);
            const std::string& getName();

            void setTransform(FTransform transformNew);
            FTransform getTransform();
            FTransform& getLocalTransform();

            glm::mat4 getModel();
            glm::mat4 getModelOld();

            void setPosition(const glm::vec3& position);
            glm::vec3 getPosition();
            const glm::vec3 getLocalPosition() const;

            void setRotation(const glm::vec3& rotation);
            glm::vec3 getRotation();
            const glm::vec3 getLocalRotation() const;

            void setScale(const glm::vec3& scale);
            glm::vec3 getScale();
            const glm::vec3 getLocalScale() const;

            void setIndex(uint32_t index);
            uint32_t getIndex();

            void setVisible(bool value);
            bool isVisible() const;
            void setEnable(bool value);
            bool isEnabled() const;

		protected:
            uint64_t objectId{ 0 };
            uint32_t objectIndex{ 0 };
            std::string srName;
            FTransform transform, transform_old;
            bool bVisible{ true };
            bool bEnable{ true };

            std::shared_ptr<CGameObject> pNull{ nullptr };
            std::shared_ptr<CGameObject> pParent;
            std::shared_ptr<CGameObject> pParentOld;
            std::unordered_map<std::string, std::shared_ptr<CGameObject>> mChilds;
		};
	}
}