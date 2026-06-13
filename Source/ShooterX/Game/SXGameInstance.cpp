// SXGameInstance.cpp

#include "SXGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "JsonObjectConverter.h"
#include "UObject/SavePackage.h"

USXGameInstance::USXGameInstance()
{
}

void USXGameInstance::Init()
{
	Super::Init();

	TCHAR ArrayOfTChar[] = TEXT("Hello, Unreal!");
	// UTF-16 인코딩의 문자열을 저장하기 위한 언리얼 표준 문자 자료형이 TCHAR.
	// TCHAR 문자열을 생성하기 위해 TEXT() 매크로를 활용.

	FString String0 = ArrayOfTChar;
	// 문자열을 좀 더 자유롭게 조작하고 싶다면 TCHAR 배열 대신 FString 자료형을 사용하면 됨. 
	// TCHAR 배열을 Wrapping한 헬퍼 클래스가 FString.
	FString String1 = FString(TEXT("Hello, Unreal!"));
	// String0와 String1은 같음.
	UE_LOG(LogTemp, Log, TEXT("String1: %s"), *String1);
	// FString에 Asterisk(*)를 붙혀줘야 TCHAR 배열이 반환됨.

	const TCHAR* PtrToTChar = *String1;
	TCHAR* RawPtrToTChar = String1.GetCharArray().GetData();

	if (String1.Contains(TEXT("unreal"), ESearchCase::IgnoreCase) == true)
	{
		int32 Index = String1.Find(TEXT("unreal"), ESearchCase::IgnoreCase);
		FString FoundedString = String1.Mid(Index);
		// "unreal" 문자열이 시작되는 곳에서부터 마지막까지 자름.
		UE_LOG(LogTemp, Log, TEXT("Founded string: %s"), *FoundedString);
	}

	int32 IntValue = 7;
	float FloatValue = 3.141592f;

	FString StringWithNumbers = FString::Printf(TEXT("int32: %d and float: %f"), IntValue, FloatValue);
	UE_LOG(LogTemp, Log, TEXT("StringWithNumber: %s"), *StringWithNumbers);

	FString IntString = FString::FromInt(IntValue);
	UE_LOG(LogTemp, Log, TEXT("IntString: %s"), *IntString);

	FString FloatString = FString::SanitizeFloat(FloatValue);
	// float 자료형의 표준은 상당히 복잡함. 이를 정돈해서 문자열로 바꿔줌.
	UE_LOG(LogTemp, Log, TEXT("FloatString: %s"), *FloatString);

	int32 IntValueFromString = FCString::Atoi(*IntString);
	// C 문자열 라이브러리 함수(atoi(), strcpy(), ...)를 제공하는 클래스 FCString.
	UE_LOG(LogTemp, Log, TEXT("IntValueFromString: %d"), IntValueFromString);

	float FloatValueFromString = FCString::Atof(*FloatString);
	UE_LOG(LogTemp, Log, TEXT("FloatValueFromString: %f"), FloatValueFromString);
}

void USXGameInstance::Shutdown()
{
	Super::Shutdown();
}
