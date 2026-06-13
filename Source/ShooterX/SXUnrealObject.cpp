// SXUnrealObject.cpp


#include "SXUnrealObject.h"

USXUnrealObject::USXUnrealObject()
{
	Name = TEXT("USXUnrealObject CDO");
}

void USXUnrealObject::HelloUnreal()
{
	UE_LOG(LogTemp, Log, TEXT("USXUnrealObject::HelloUnreal() has been called."));
}
