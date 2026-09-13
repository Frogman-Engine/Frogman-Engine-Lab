// Copyright © from 2024 to present, UNKNOWN STRYKER (Hojin Lee / Joey). All Rights Reserved. 
#include <FE/framework/reflection/private/load_reflection_data.hxx> 
#include <FE/framework.hxx> 
#include <memory>

#include <C:\Users\leeho\OneDrive\문서\GitHub\Frogman-Engine\SDK\Tests\FHT-Test\ReflextreeParseTest2.hpp>




void load_reflection_data()
{
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::GlobalEntity*(::GlobalEntity*)> >("construct ::GlobalEntity", &::std::construct_at<::GlobalEntity>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::GlobalEntity*)> >("destruct ::GlobalEntity", &::std::destroy_at<::GlobalEntity>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::GlobalRenderComponent*(::GlobalRenderComponent*)> >("construct ::GlobalRenderComponent", &::std::construct_at<::GlobalRenderComponent>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::GlobalRenderComponent*)> >("destruct ::GlobalRenderComponent", &::std::destroy_at<::GlobalRenderComponent>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::SimpleClass*(::FE::Test::SimpleClass*)> >("construct ::FE::Test::SimpleClass", &::std::construct_at<::FE::Test::SimpleClass>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::SimpleClass*)> >("destruct ::FE::Test::SimpleClass", &::std::destroy_at<::FE::Test::SimpleClass>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::PublicClass*(::FE::Test::PublicClass*)> >("construct ::FE::Test::PublicClass", &::std::construct_at<::FE::Test::PublicClass>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::PublicClass*)> >("destruct ::FE::Test::PublicClass", &::std::destroy_at<::FE::Test::PublicClass>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::FullAccessClass*(::FE::Test::FullAccessClass*)> >("construct ::FE::Test::FullAccessClass", &::std::construct_at<::FE::Test::FullAccessClass>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::FullAccessClass*)> >("destruct ::FE::Test::FullAccessClass", &::std::destroy_at<::FE::Test::FullAccessClass>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::DerivedClass*(::FE::Test::DerivedClass*)> >("construct ::FE::Test::DerivedClass", &::std::construct_at<::FE::Test::DerivedClass>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::DerivedClass*)> >("destruct ::FE::Test::DerivedClass", &::std::destroy_at<::FE::Test::DerivedClass>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::FinalClass*(::FE::Test::FinalClass*)> >("construct ::FE::Test::FinalClass", &::std::construct_at<::FE::Test::FinalClass>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::FinalClass*)> >("destruct ::FE::Test::FinalClass", &::std::destroy_at<::FE::Test::FinalClass>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::NonCopyableClass*(::FE::Test::NonCopyableClass*)> >("construct ::FE::Test::NonCopyableClass", &::std::construct_at<::FE::Test::NonCopyableClass>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::NonCopyableClass*)> >("destruct ::FE::Test::NonCopyableClass", &::std::destroy_at<::FE::Test::NonCopyableClass>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::ClassWithFriend*(::FE::Test::ClassWithFriend*)> >("construct ::FE::Test::ClassWithFriend", &::std::construct_at<::FE::Test::ClassWithFriend>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::ClassWithFriend*)> >("destruct ::FE::Test::ClassWithFriend", &::std::destroy_at<::FE::Test::ClassWithFriend>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::ThreadLocalClass*(::FE::Test::ThreadLocalClass*)> >("construct ::FE::Test::ThreadLocalClass", &::std::construct_at<::FE::Test::ThreadLocalClass>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::ThreadLocalClass*)> >("destruct ::FE::Test::ThreadLocalClass", &::std::destroy_at<::FE::Test::ThreadLocalClass>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::ConstClass*(::FE::Test::ConstClass*)> >("construct ::FE::Test::ConstClass", &::std::construct_at<::FE::Test::ConstClass>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::ConstClass*)> >("destruct ::FE::Test::ConstClass", &::std::destroy_at<::FE::Test::ConstClass>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::NoexceptClass*(::FE::Test::NoexceptClass*)> >("construct ::FE::Test::NoexceptClass", &::std::construct_at<::FE::Test::NoexceptClass>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::NoexceptClass*)> >("destruct ::FE::Test::NoexceptClass", &::std::destroy_at<::FE::Test::NoexceptClass>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::VirtualBase*(::FE::Test::VirtualBase*)> >("construct ::FE::Test::VirtualBase", &::std::construct_at<::FE::Test::VirtualBase>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::VirtualBase*)> >("destruct ::FE::Test::VirtualBase", &::std::destroy_at<::FE::Test::VirtualBase>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::ConcreteImplementation*(::FE::Test::ConcreteImplementation*)> >("construct ::FE::Test::ConcreteImplementation", &::std::construct_at<::FE::Test::ConcreteImplementation>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::ConcreteImplementation*)> >("destruct ::FE::Test::ConcreteImplementation", &::std::destroy_at<::FE::Test::ConcreteImplementation>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::OperatorClass*(::FE::Test::OperatorClass*)> >("construct ::FE::Test::OperatorClass", &::std::construct_at<::FE::Test::OperatorClass>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::OperatorClass*)> >("destruct ::FE::Test::OperatorClass", &::std::destroy_at<::FE::Test::OperatorClass>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::CallingConventionClass*(::FE::Test::CallingConventionClass*)> >("construct ::FE::Test::CallingConventionClass", &::std::construct_at<::FE::Test::CallingConventionClass>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::CallingConventionClass*)> >("destruct ::FE::Test::CallingConventionClass", &::std::destroy_at<::FE::Test::CallingConventionClass>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::Nested::NestedClass*(::FE::Test::Nested::NestedClass*)> >("construct ::FE::Test::Nested::NestedClass", &::std::construct_at<::FE::Test::Nested::NestedClass>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::Nested::NestedClass*)> >("destruct ::FE::Test::Nested::NestedClass", &::std::destroy_at<::FE::Test::Nested::NestedClass>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::Nested::Deep::DeepClass*(::FE::Test::Nested::Deep::DeepClass*)> >("construct ::FE::Test::Nested::Deep::DeepClass", &::std::construct_at<::FE::Test::Nested::Deep::DeepClass>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::Nested::Deep::DeepClass*)> >("destruct ::FE::Test::Nested::Deep::DeepClass", &::std::destroy_at<::FE::Test::Nested::Deep::DeepClass>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::EdgeCases::OuterClass*(::FE::Test::EdgeCases::OuterClass*)> >("construct ::FE::Test::EdgeCases::OuterClass", &::std::construct_at<::FE::Test::EdgeCases::OuterClass>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::EdgeCases::OuterClass*)> >("destruct ::FE::Test::EdgeCases::OuterClass", &::std::destroy_at<::FE::Test::EdgeCases::OuterClass>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::EdgeCases::ComplexPointers*(::FE::Test::EdgeCases::ComplexPointers*)> >("construct ::FE::Test::EdgeCases::ComplexPointers", &::std::construct_at<::FE::Test::EdgeCases::ComplexPointers>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::EdgeCases::ComplexPointers*)> >("destruct ::FE::Test::EdgeCases::ComplexPointers", &::std::destroy_at<::FE::Test::EdgeCases::ComplexPointers>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::GlobalPoint*(::GlobalPoint*)> >("construct ::GlobalPoint", &::std::construct_at<::GlobalPoint>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::GlobalPoint*)> >("destruct ::GlobalPoint", &::std::destroy_at<::GlobalPoint>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::GlobalVector*(::GlobalVector*)> >("construct ::GlobalVector", &::std::construct_at<::GlobalVector>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::GlobalVector*)> >("destruct ::GlobalVector", &::std::destroy_at<::GlobalVector>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::GlobalTransform*(::GlobalTransform*)> >("construct ::GlobalTransform", &::std::construct_at<::GlobalTransform>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::GlobalTransform*)> >("destruct ::GlobalTransform", &::std::destroy_at<::GlobalTransform>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::Point2D*(::FE::Test::Point2D*)> >("construct ::FE::Test::Point2D", &::std::construct_at<::FE::Test::Point2D>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::Point2D*)> >("destruct ::FE::Test::Point2D", &::std::destroy_at<::FE::Test::Point2D>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::Rectangle*(::FE::Test::Rectangle*)> >("construct ::FE::Test::Rectangle", &::std::construct_at<::FE::Test::Rectangle>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::Rectangle*)> >("destruct ::FE::Test::Rectangle", &::std::destroy_at<::FE::Test::Rectangle>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::Point3D*(::FE::Test::Point3D*)> >("construct ::FE::Test::Point3D", &::std::construct_at<::FE::Test::Point3D>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::Point3D*)> >("destruct ::FE::Test::Point3D", &::std::destroy_at<::FE::Test::Point3D>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::Vector3D*(::FE::Test::Vector3D*)> >("construct ::FE::Test::Vector3D", &::std::construct_at<::FE::Test::Vector3D>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::Vector3D*)> >("destruct ::FE::Test::Vector3D", &::std::destroy_at<::FE::Test::Vector3D>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::Transform*(::FE::Test::Transform*)> >("construct ::FE::Test::Transform", &::std::construct_at<::FE::Test::Transform>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::Transform*)> >("destruct ::FE::Test::Transform", &::std::destroy_at<::FE::Test::Transform>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::PhysicsTransform*(::FE::Test::PhysicsTransform*)> >("construct ::FE::Test::PhysicsTransform", &::std::construct_at<::FE::Test::PhysicsTransform>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::PhysicsTransform*)> >("destruct ::FE::Test::PhysicsTransform", &::std::destroy_at<::FE::Test::PhysicsTransform>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::Configuration*(::FE::Test::Configuration*)> >("construct ::FE::Test::Configuration", &::std::construct_at<::FE::Test::Configuration>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::Configuration*)> >("destruct ::FE::Test::Configuration", &::std::destroy_at<::FE::Test::Configuration>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::Flags*(::FE::Test::Flags*)> >("construct ::FE::Test::Flags", &::std::construct_at<::FE::Test::Flags>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::Flags*)> >("destruct ::FE::Test::Flags", &::std::destroy_at<::FE::Test::Flags>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::PointerData*(::FE::Test::PointerData*)> >("construct ::FE::Test::PointerData", &::std::construct_at<::FE::Test::PointerData>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::PointerData*)> >("destruct ::FE::Test::PointerData", &::std::destroy_at<::FE::Test::PointerData>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::Nested::NestedPoint*(::FE::Test::Nested::NestedPoint*)> >("construct ::FE::Test::Nested::NestedPoint", &::std::construct_at<::FE::Test::Nested::NestedPoint>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::Nested::NestedPoint*)> >("destruct ::FE::Test::Nested::NestedPoint", &::std::destroy_at<::FE::Test::Nested::NestedPoint>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::Nested::Deep::DeepStruct*(::FE::Test::Nested::Deep::DeepStruct*)> >("construct ::FE::Test::Nested::Deep::DeepStruct", &::std::construct_at<::FE::Test::Nested::Deep::DeepStruct>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::Nested::Deep::DeepStruct*)> >("destruct ::FE::Test::Nested::Deep::DeepStruct", &::std::destroy_at<::FE::Test::Nested::Deep::DeepStruct>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::EdgeCases::OuterStruct*(::FE::Test::EdgeCases::OuterStruct*)> >("construct ::FE::Test::EdgeCases::OuterStruct", &::std::construct_at<::FE::Test::EdgeCases::OuterStruct>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::EdgeCases::OuterStruct*)> >("destruct ::FE::Test::EdgeCases::OuterStruct", &::std::destroy_at<::FE::Test::EdgeCases::OuterStruct>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::EdgeCases::ArrayStruct*(::FE::Test::EdgeCases::ArrayStruct*)> >("construct ::FE::Test::EdgeCases::ArrayStruct", &::std::construct_at<::FE::Test::EdgeCases::ArrayStruct>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::EdgeCases::ArrayStruct*)> >("destruct ::FE::Test::EdgeCases::ArrayStruct", &::std::destroy_at<::FE::Test::EdgeCases::ArrayStruct>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<::FE::Test::EdgeCases::FunctionPointers*(::FE::Test::EdgeCases::FunctionPointers*)> >("construct ::FE::Test::EdgeCases::FunctionPointers", &::std::construct_at<::FE::Test::EdgeCases::FunctionPointers>);
    ::FE::framework::framework_base::get_framework().get_method_reflection().register_task< ::FE::c_style_task<void(::FE::Test::EdgeCases::FunctionPointers*)> >("destruct ::FE::Test::EdgeCases::FunctionPointers", &::std::destroy_at<::FE::Test::EdgeCases::FunctionPointers>);

    ::FE::framework::framework_base::get_framework().get_enum_reflection().register_enum_struct< ::GlobalStatus >("::GlobalStatus",
    {
        { ::GlobalStatus::Inactive, "Inactive" },
        { ::GlobalStatus::Active, "Active" },
        { ::GlobalStatus::Pending, "Pending" }
    });

    ::FE::framework::framework_base::get_framework().get_enum_reflection().register_enum_struct< ::GlobalErrorCode >("::GlobalErrorCode",
    {
        { ::GlobalErrorCode::Success, "Success" },
        { ::GlobalErrorCode::GeneralError, "GeneralError" },
        { ::GlobalErrorCode::InvalidInput, "InvalidInput" },
        { ::GlobalErrorCode::OutOfMemory, "OutOfMemory" }
    });

    ::FE::framework::framework_base::get_framework().get_enum_reflection().register_enum_struct< ::GlobalPriority >("::GlobalPriority",
    {
        { ::GlobalPriority::Low, "Low" },
        { ::GlobalPriority::Medium, "Medium" },
        { ::GlobalPriority::High, "High" }
    });

    ::FE::framework::framework_base::get_framework().get_enum_reflection().register_enum_struct< ::FE::Test::Color >("::FE::Test::Color",
    {
        { ::FE::Test::Color::Red, "Red" },
        { ::FE::Test::Color::Green, "Green" },
        { ::FE::Test::Color::Blue, "Blue" },
        { ::FE::Test::Color::Yellow, "Yellow" }
    });

    ::FE::framework::framework_base::get_framework().get_enum_reflection().register_enum_struct< ::FE::Test::ByteStatus >("::FE::Test::ByteStatus",
    {
        { ::FE::Test::ByteStatus::Idle, "Idle" },
        { ::FE::Test::ByteStatus::Running, "Running" },
        { ::FE::Test::ByteStatus::Paused, "Paused" },
        { ::FE::Test::ByteStatus::Stopped, "Stopped" },
        { ::FE::Test::ByteStatus::Error, "Error" }
    });

    ::FE::framework::framework_base::get_framework().get_enum_reflection().register_enum_struct< ::FE::Test::LongCode >("::FE::Test::LongCode",
    {
        { ::FE::Test::LongCode::MinValue, "MinValue" },
        { ::FE::Test::LongCode::Zero, "Zero" },
        { ::FE::Test::LongCode::MaxValue, "MaxValue" }
    });

    ::FE::framework::framework_base::get_framework().get_enum_reflection().register_enum_struct< ::FE::Test::SingleValue >("::FE::Test::SingleValue",
    {
        { ::FE::Test::SingleValue::Only, "Only" }
    });

    ::FE::framework::framework_base::get_framework().get_enum_reflection().register_enum_struct< ::FE::Test::EmptyEnum >("::FE::Test::EmptyEnum",
    {
        { ::FE::Test::EmptyEnum::_, "_" }
    });

    ::FE::framework::framework_base::get_framework().get_enum_reflection().register_enum_struct< ::FE::Test::Nested::NestedColor >("::FE::Test::Nested::NestedColor",
    {
        { ::FE::Test::Nested::NestedColor::Cyan, "Cyan" },
        { ::FE::Test::Nested::NestedColor::Magenta, "Magenta" },
        { ::FE::Test::Nested::NestedColor::Yellow, "Yellow" },
        { ::FE::Test::Nested::NestedColor::Black, "Black" }
    });

    ::FE::framework::framework_base::get_framework().get_enum_reflection().register_enum_struct< ::FE::Test::Nested::Deep::DeepEnum >("::FE::Test::Nested::Deep::DeepEnum",
    {
        { ::FE::Test::Nested::Deep::DeepEnum::Value1, "Value1" },
        { ::FE::Test::Nested::Deep::DeepEnum::Value2, "Value2" }
    });

    ::FE::framework::framework_base::get_framework().get_enum_reflection().register_enum_struct< ::FE::Test::EdgeCases::LargeEnum >("::FE::Test::EdgeCases::LargeEnum",
    {
        { ::FE::Test::EdgeCases::LargeEnum::Zero, "Zero" },
        { ::FE::Test::EdgeCases::LargeEnum::Small, "Small" },
        { ::FE::Test::EdgeCases::LargeEnum::Medium, "Medium" },
        { ::FE::Test::EdgeCases::LargeEnum::Large, "Large" },
        { ::FE::Test::EdgeCases::LargeEnum::VeryLarge, "VeryLarge" }
    });

    ::FE::framework::framework_base::get_framework().get_enum_reflection().register_enum_struct< ::FE::Test::EdgeCases::Fake >("::FE::Test::EdgeCases::Fake",
    {
        { ::FE::Test::EdgeCases::Fake::NerfThis, "NerfThis" }
    });
}
