// SXEagle.cpp


#include "Example/SXEagle.h"

USXEagle::USXEagle()
{
	Name = TEXT("Eagle");
}

void USXEagle::Fly()
{
	UE_LOG(LogTemp, Log, TEXT("%s is now flying."), *Name);
}

