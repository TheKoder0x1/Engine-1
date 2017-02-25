//
//  Author: Shervin Aflatooni
//

#include "PhysicsManager.h"

#include <glm/gtx/norm.hpp>

#include <algorithm>

PhysicsManager::PhysicsManager(void)
{
  m_collisionConfiguration = new btDefaultCollisionConfiguration();
  m_dispatcher = new	btCollisionDispatcher(m_collisionConfiguration);
	m_overlappingPairCache = new btDbvtBroadphase();
	m_solver = new btSequentialImpulseConstraintSolver;
	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_overlappingPairCache, m_solver, m_collisionConfiguration);

	m_dynamicsWorld->setGravity(btVector3(0,-10,0));



  /*{
    btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(0.), btScalar(50.)));

    btTransform groundTransform;
    groundTransform.setIdentity();
    groundTransform.setOrigin(btVector3(0, -2, 0));

    btScalar mass(0.);

    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0, 0, 0);
    if (isDynamic)
      groundShape->calculateLocalInertia(mass, localInertia);

    //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    //add the body to the dynamics world
    m_dynamicsWorld->addRigidBody(body);
  }*/
}

PhysicsManager::~PhysicsManager(void)
{
  delete m_dynamicsWorld;
  delete m_solver;
  delete m_overlappingPairCache;
  delete m_dispatcher;
  delete m_collisionConfiguration;
}

void PhysicsManager::registerCollider(std::shared_ptr<Sphere> sphere)
{
  m_colliders.push_back(sphere);
}

void PhysicsManager::registerCollider2(btRigidBody *rigidBody)
{
  m_dynamicsWorld->addRigidBody(rigidBody);
}

void PhysicsManager::deregisterCollider(std::shared_ptr<Sphere> sphere)
{
  m_colliders.erase(std::remove(m_colliders.begin(), m_colliders.end(), sphere), m_colliders.end());
}

void PhysicsManager::deregisterCollider2(btRigidBody *rigidBody)
{
  m_dynamicsWorld->removeRigidBody(rigidBody);
}

void PhysicsManager::tick(std::chrono::microseconds delta) {
  m_dynamicsWorld->stepSimulation(std::chrono::duration_cast<std::chrono::duration<float>>(delta).count());
}

Entity *PhysicsManager::pick(Ray *ray) const
{
  glm::vec3 intersectionPosition;
  float closest = std::numeric_limits<float>::max();
  Entity *entity = nullptr;

  for (unsigned int i = 0; i < m_colliders.size(); i++)
  {
    if (ray->intersects(m_colliders[i].get(), intersectionPosition)) {
      float length = glm::length2(intersectionPosition - ray->getPosition());
      if (length < closest)
        entity = m_colliders[i]->getParent();
    }
  }

  return entity;
}
