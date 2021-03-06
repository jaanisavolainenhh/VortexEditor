#include "entity.h"

Entity::Entity(const Vortex2D::Renderer::Device& device,
               const glm::ivec2& size,
               float scale,
               ShapeType type,
               std::unique_ptr<Vortex2D::Renderer::Shape> shape,
               Vortex2D::Renderer::RenderCommand cmd,
               b2World& box2dWorld)
    : mScale(scale), mShapeType(type), mShape(std::move(shape)), mCmd(std::move(cmd))
{
  mShapeType.match(
      [&](const Circle& circle) {
        auto radius = circle.mRadius / scale;
        mRigidbody = std::make_unique<CircleRigidbody>(device,
                                                       size,
                                                       box2dWorld,
                                                       b2_staticBody,
                                                       Vortex2D::Fluid::RigidBody::Type::eStatic,
                                                       radius);
      },
      [&](const Rectangle& rectangle) {
        auto halfRectSize = rectangle.mSize / (glm::vec2(2.0f) * scale);
        mRigidbody = std::make_unique<RectangleRigidbody>(device,
                                                          size,
                                                          box2dWorld,
                                                          b2_staticBody,
                                                          Vortex2D::Fluid::RigidBody::Type::eStatic,
                                                          halfRectSize);
      },
      [&](const Polygon& /*polygon*/) {

      });

  mRigidbody->SetTransform(mShape->Position / mScale, mShape->Rotation);
  mRigidbody->mBody->SetUserData(this);
}

void Entity::SetTransform(const glm::vec2& pos, float angle)
{
  mShape->Position = pos;
  mShape->Rotation = angle;
  mRigidbody->SetTransform(pos / mScale, angle);
}

void Entity::UpdateTransform()
{
  auto pos = mScale * mRigidbody->mRigidbody->Position;
  auto angle = mRigidbody->mRigidbody->Rotation;

  mShape->Position = pos;
  mShape->Rotation = angle;
}

bool IsValid(const ShapeType& type)
{
  return type.match([&](const Circle& circle) { return circle.mRadius > 0.0f; },
                    [&](const Rectangle& rectangle) {
                      return rectangle.mSize.x > 0.0f && rectangle.mSize.y > 0.0f;
                    },
                    [&](const Polygon& /*polygon*/) { return false; });
}
