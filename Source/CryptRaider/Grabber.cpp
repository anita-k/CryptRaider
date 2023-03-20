// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	// const UPhysicsHandleComponent* PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	// if (PhysicsHandle != nullptr)
	// {
	// 	const FString PhysicsHandleName = PhysicsHandle->GetName();
	//
	// 	UE_LOG(LogTemp, Display, TEXT("Got Physics Handle: %s"), *PhysicsHandleName);
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Display, TEXT("Got Physics Handle Found"));
	// }	
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();
	// if (PhysicsHandle == nullptr)
	// {
	// 	return;
	// }

	if (PhysicsHandle && PhysicsHandle->GetGrabbedComponent())
	{
		FVector TargetLocation = GetComponentLocation() + GetForwardVector() * HoldDistance;
		PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, GetComponentRotation());
	}
}
	// FRotator MyRotation = GetComponentRotation();
	// FString RotationString = MyRotation.ToCompactString();
	// UE_LOG(LogTemp, Display, TEXT("Grabber Rotation: %s"), *RotationString);
	//
	// float Time = GetWorld()->TimeSeconds;
	// UE_LOG(LogTemp, Display, TEXT("Current time is: %f"), Time);


void UGrabber::Grab()
{
	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();
	if (PhysicsHandle == nullptr)
	{
		return;
	}

	UWorld* MyWorld = GetWorld();

	FVector Start = GetComponentLocation();
	FVector End = Start + GetForwardVector() * MaxGrabDistance;
	// DrawDebugLine(MyWorld, Start, End, FColor::Green, false, 5, 0, 2);

	DrawDebugSphere(MyWorld, End, 10, 10, FColor::Blue, false, 5);

	FCollisionShape Sphere = FCollisionShape::MakeSphere(GrabRadius);

	FHitResult HitResult;

	bool HasHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		End,
		FQuat::Identity,
		ECC_GameTraceChannel2,
		Sphere);
	if (HasHit)
	{
		UPrimitiveComponent* HitComponent = HitResult.GetComponent();
		HitComponent->WakeAllRigidBodies();
		HitResult.GetActor()->Tags.Add("Grabbed");
			UE_LOG(LogTemp, Display, TEXT("Grabbed"));
		PhysicsHandle->GrabComponentAtLocationWithRotation(
			HitComponent,
			NAME_None,
			HitResult.ImpactPoint,
			GetComponentRotation()
		);
	}
	
	// {
		DrawDebugSphere(MyWorld, HitResult.Location, 10, 10, FColor::Green, false, 5);
		DrawDebugSphere(MyWorld, HitResult.ImpactPoint, 10, 10, FColor::Red, false, 5);
	// 	AActor* HitActor = HitResult.GetActor();
	// 	
	// 	UE_LOG(LogTemp, Display, TEXT("Hit Actor: %s"), *HitActor->GetActorNameOrLabel());

	// else
	// {
	// 	UE_LOG(LogTemp, Display, TEXT("No actor hit"));
	// }
}

void UGrabber::Release()
{
	// UE_LOG(LogTemp, Display, TEXT("Released Grabber"));
	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();
	// if (PhysicsHandle == nullptr)
	// {
	// 	return;
	// }

	if (PhysicsHandle && PhysicsHandle->GetGrabbedComponent())
	{
		// PhysicsHandle->GetGrabbedComponent()->WakeAllRigidBodies();
		AActor* GrabbedActor = PhysicsHandle->GetGrabbedComponent()->GetOwner();
		GrabbedActor->Tags.Remove("Grabbed");
		UE_LOG(LogTemp, Display, TEXT("Released"));
		PhysicsHandle->ReleaseComponent();
	}
}

// void UGrabber::PrintDamage(float Damage)
// {
// 	UE_LOG(LogTemp, Display, TEXT("DamageRef: %f, Damage: %f"), Damage)
// }

UPhysicsHandleComponent* UGrabber::GetPhysicsHandle() const
{
	UPhysicsHandleComponent* Result = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (Result == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Grabber requires a UPhysicsHandleComponent."));
	}
	return Result;
}
