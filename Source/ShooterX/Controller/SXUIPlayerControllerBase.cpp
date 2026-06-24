// SXUIPlayerControllerBase.cpp


#include "Controller/SXUIPlayerControllerBase.h"
#include "Blueprint/UserWidget.h"
#include "ShooterX.h"

void ASXUIPlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();

	FString NetModeString = ShooterXFunctionLibrary::GetNetModeString(this);
	ShooterXFunctionLibrary::MyPrintString(this, NetModeString, 10.f);
		// 문제 상황이 생기면, 위와 같은 로깅 함수로 다양한 변수의 값들과 함수이름을 확인해서 
		// 문제의 원인을 적극적으로 찾아보세요!

	if (IsLocalController() == false)
	{
		return;
	}

	if (true == ::IsValid(UIWidgetClass))
	{
		UIWidgetInstance = CreateWidget<UUserWidget>(this, UIWidgetClass);
		// CreateWidget()이 호출될 때 UIWidgetInstance->NativeOnInitialize() 함수가 호출됨.
		if (true == ::IsValid(UIWidgetInstance))
		{
			UIWidgetInstance->AddToViewport();
			// AddToViewport()가 호출 될 때 UIWidgetInstance->NativeConstruct() 함수가 호출됨.

			FInputModeUIOnly Mode;
			Mode.SetWidgetToFocus(UIWidgetInstance->GetCachedWidget());
			SetInputMode(Mode);

			bShowMouseCursor = true;
		}
	}
}
