// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomTriggerBox.h"

#include "Components/BoxComponent.h"


// Sets default values
ACustomTriggerBox::ACustomTriggerBox() : AActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>("Box Collision");
	BoxCollision->SetupAttachment(RootComponent);

	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxCollision->SetCollisionProfileName(TEXT("TriggerBox"));

	// Bind overlap events
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ACustomTriggerBox::OnOverlapBegin);
}

//Call OnTrigger() when overlap occured
void ACustomTriggerBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                       const FHitResult& SweepResult)
{
	OnTrigger();
}
