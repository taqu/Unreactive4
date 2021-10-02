#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UNRX4/UNRX4.h"
#include "UNRX4/UNRX4IObservable.h"
#include "UNRX4/UNRX4IObserver.h"
#include "TestUserWidget.generated.h"

class FuncTestObserver: public UNRX4IObserver<int32>
{
public:
    void next(int32 id) override
    {
        UE_LOG(LogTemp, Log, TEXT("next:%d"), id);
    }

    void error(unrx4::error_code_type errorCode) override
    {
        UE_LOG(LogTemp, Log, TEXT("error:%d"), errorCode);
    }

    void completed() override
    {
        UE_LOG(LogTemp, Log, TEXT("completed"));
    }
};

/**
 *
 */
UCLASS()
class UNREACTIVE4_API UTestUserWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable, Category = "UITest")
    void invokeClick(int32 id);
private:
    void onClick(int32 id);

    UNRX4Function<void(int32)> action_;
    TArray<FUNRX4OnClickDelegate> onClickDelegates_;
    unrx4_unique_ptr<UNRX4IObservable<int32>> observable_;
    FuncTestObserver observer_;
};
