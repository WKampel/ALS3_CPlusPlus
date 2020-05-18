
#include "CoreMinimal.h"
#include "IdleEntryState.generated.h"

UENUM(BlueprintType)
enum EIdleEntryState
{
	N_Idle     UMETA(DisplayName = "N_Idle"),
	LF_Idle     UMETA(DisplayName = "LF_Idle"),
	RF_Idle     UMETA(DisplayName = "RF_Idle"),
	CLF_Idle     UMETA(DisplayName = "CLF_Idle"),
	CRF_Idle     UMETA(DisplayName = "CRF_Idle"),
};