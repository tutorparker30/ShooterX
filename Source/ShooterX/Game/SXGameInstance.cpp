// SXGameInstance.cpp

#include "SXGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"

USXGameInstance::USXGameInstance()
{
	UE_LOG(LogTemp, Log, TEXT("USXGameInstance::USXGameInstance() has been called."));
}

void USXGameInstance::Init()
{
	Super::Init();

	UE_LOG(LogTemp, Log, TEXT("USXGameInstance::Init() has been called."));

	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("USXGameInstance::Init() has been called."));
		// UKismetSystemLibrary::PrintString() 함수의 첫 매개변수 이름은 "WorldContextObject"
		// Context란, 문맥상 ~될만한거 라고 생각하면 좋음. 여기서는 "월드 개체를 만들만한 거"라고 해석.
		// 따라서 GetWorld()를 통해서 진짜 World 개체를 넘겨줘도 되지만
		// this(USGameInstance 개체)를 전달해서 this가 속한 월드 개체를 구하게끔 해도된다는 뜻.
}

void USXGameInstance::Shutdown()
{
	Super::Shutdown();

	UE_LOG(LogTemp, Log, TEXT("USXGameInstance::Shutdown() has been called."));
}

