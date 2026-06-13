// SXGameInstance.cpp

#include "SXGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
//#include "SXUnrealObject.h"
#include "Example/SXFlyable.h"
#include "Example/SXPigeon.h"
#include "Example/SXEagle.h"

USXGameInstance::USXGameInstance()
{
	//Name = TEXT("SXGameInstance's Class Default Object");
}

void USXGameInstance::Init()
{
	Super::Init();

	/*
	USXUnrealObject* USXObject01 = NewObject<USXUnrealObject>();

	UE_LOG(LogTemp, Log, TEXT("USXObject01's Name: %s"), *USXObject01->GetObjectName());

	FProperty* NameProperty = USXUnrealObject::StaticClass()->FindPropertyByName(TEXT("Name"));
	FString CompiletimeUSObjectName;
	if (nullptr != NameProperty)
	{
		NameProperty->GetValue_InContainer(USXObject01, &CompiletimeUSObjectName);
		UE_LOG(LogTemp, Log, TEXT("CompiletimeUSObjectName: %s"), *CompiletimeUSObjectName);
	}

	USXObject01->HelloUnreal();

	UFunction* HelloUnrealFunction = USXObject01->GetClass()->FindFunctionByName(TEXT("HelloUnreal"));
	if (nullptr != HelloUnrealFunction)
	{
		USXObject01->ProcessEvent(HelloUnrealFunction, nullptr);
	}
	*/

	USXPigeon* Pigeon1 = NewObject<USXPigeon>();
	if (IsValid(Pigeon1) == true)
	{
		Pigeon1->Fly();
	}

	USXEagle* Eagle1 = NewObject<USXEagle>();
	if (IsValid(Eagle1) == true)
	{
		Eagle1->Fly();
	}

	TArray<ISXFlyable*> Birds;
	Birds.Reserve(10);

	if (Pigeon1->GetClass()->ImplementsInterface(USXFlyable::StaticClass()) == true)
	{
		ISXFlyable* Bird1 = Cast<ISXFlyable>(Pigeon1);
		Birds.Emplace(Bird1);
	}

	if (Eagle1->GetClass()->ImplementsInterface(USXFlyable::StaticClass()) == true)
	{
		ISXFlyable* Bird2 = Cast<ISXFlyable>(Eagle1);
		Birds.Emplace(Bird2);
	}

	for (ISXFlyable* Bird : Birds)
	{
		Bird->Fly();
		// 다형성 예시. 같은 클래스(ISXFlyable)지만 다른 행동(Pigeon is ~, Eagle is ~)을 함.
		// 동시에 의존성 디커플링이기도 함. SXGameInstance 클래스는 어떻게 나는지, 실제론 어떤 클래스인지 알필요 없음.
		// 날기만 하면 됨(Fly() 함수를 호출 할 수 있기만 하면 됨.)
	}
}

void USXGameInstance::Shutdown()
{
	Super::Shutdown();
}

