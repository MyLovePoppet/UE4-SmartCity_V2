// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class SMARTCITY_API MyFVector
{
public:
    const static double MyKINDA_SMALL_NUMBER;
    const static double MySMALL_NUMBER;

    static double Abs(double X);

    static double Max(double X, double Y);

    static double Min(double X, double Y);

    static double InvSqrt(double X);

    static double PointDistToLine(const MyFVector& Point, const MyFVector& Direction, const MyFVector& Origin);

    static double Sqrt(double X);
    static double Square(double X);
public:
    /** Vector's X component. */
    double X;

    /** Vector's Y component. */
    double Y;

    /** Vector's Z component. */
    double Z;

    /** A zero vector (0,0,0) */
    static const MyFVector ZeroVector;

    /** One vector (1,1,1) */
    static const MyFVector OneVector;

    /** Unreal up vector (0,0,1) */
    static const MyFVector UpVector;

    /** Unreal down vector (0,0,-1) */
    static const MyFVector DownVector;

    /** Unreal forward vector (1,0,0) */
    static const MyFVector ForwardVector;

    /** Unreal backward vector (-1,0,0) */
    static const MyFVector BackwardVector;

    /** Unreal right vector (0,1,0) */
    static const MyFVector RightVector;

    /** Unreal left vector (0,-1,0) */
    static const MyFVector LeftVector;
public:
    MyFVector(double InX, double InY, double InZ);

    MyFVector& operator=(const MyFVector& Other);

    MyFVector operator^(const MyFVector& V) const;

    static MyFVector CrossProduct(const MyFVector& A, const MyFVector& B);

    double operator|(const MyFVector& V) const;

    static double DotProduct(const MyFVector& A, const MyFVector& B);

    MyFVector operator+(const MyFVector& V) const;

    MyFVector operator-(const MyFVector& V) const;

    MyFVector operator-(double Bias) const;

    MyFVector operator+(double Bias) const;

    MyFVector operator*(double Scale) const;

    MyFVector operator/(double Scale) const;

    MyFVector operator*(const MyFVector& V) const;

    MyFVector operator/(const MyFVector& V) const;

    bool operator==(const MyFVector& V) const;

    bool operator!=(const MyFVector& V) const;

    bool Equals(const MyFVector& V, double Tolerance = KINDA_SMALL_NUMBER) const;

    MyFVector operator-() const;

    MyFVector operator+=(const MyFVector& V);

    MyFVector operator-=(const MyFVector& V);

    MyFVector operator*=(double Scale);

    MyFVector operator/=(double V);

    MyFVector operator*=(const MyFVector& V);

    MyFVector operator/=(const MyFVector& V);

    double& operator[](int Index);

    double operator[](int Index) const;


    void Set(double InX, double InY, double InZ);

    double GetMax() const;

    double GetAbsMax() const;

    double GetMin() const;

    double GetAbsMin() const;

    MyFVector GetAbs() const;

    MyFVector GetSafeNormal(double Tolerance = MySMALL_NUMBER) const;

    MyFVector Projection() const;

    double Size() const;
    static double DistSquared(const MyFVector& V1, const MyFVector& V2);

    static double Dist(const MyFVector& V1, const MyFVector& V2);

    static MyFVector FromFVector(const FVector& Other);
    
    static FVector ToFVector(const MyFVector& Other);
    static FVector UnrotateVector(const FQuat& Quat,const FVector &Vector);
};
