#include "TestUserWidget.h"
#include "UNRX4/UNRX4Observable.h"

void UTestUserWidget::NativeConstruct()
{
    Super::NativeConstruct();

    observable_ = UNRX4Observable::fromEvent(action_);
    observable_->subscribe(&observer_);
}

void UTestUserWidget::invokeClick(int32 id)
{
    action_(id);
}

void UTestUserWidget::onClick(int32 id)
{
}

