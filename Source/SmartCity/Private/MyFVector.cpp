// Fill out your copyright notice in the Description page of Project Settings.


#include "MyFVector.h"
const double MyFVector::MyKINDA_SMALL_NUMBER = 1e-4;
const double MyFVector::MySMALL_NUMBER = 1e-8;

double MyFVector::Abs(double X)
{
    return X > 0 ? X : -1.0 * X;
}

double MyFVector::Max(double X, double Y)
{
    return X > Y ? X : Y;
}

double MyFVector::Min(double X, double Y)
{
    return X < Y ? X : Y;
}

double MyFVector::InvSqrt(double X)
{
    return 1.0 / std::sqrt(X);
}

double MyFVector::PointDistToLine(const MyFVector& Point, const MyFVector& Direction, const MyFVector& Origin)
{
    const MyFVector SafeDir = Direction.GetSafeNormal();
    const MyFVector OutClosestPoint = Origin + (SafeDir * ((Point - Origin) | SafeDir));
    return (OutClosestPoint - Point).Size();
}

double MyFVector::Sqrt(double X)
{
    return std::sqrt(X);
}

double MyFVector::Square(double X)
{
    return X * X;
}

const MyFVector MyFVector::ZeroVector(0.0, 0.0, 0.0);
const MyFVector MyFVector::OneVector(1.0, 1.0, 1.0);
const MyFVector MyFVector::UpVector(0.0, 0.0, 1.0);
const MyFVector MyFVector::DownVector(0.0, 0.0, -1.0);
const MyFVector MyFVector::ForwardVector(1.0, 0.0, 0.0);
const MyFVector MyFVector::BackwardVector(-1.0, 0.0, 0.0);
const MyFVector MyFVector::RightVector(0.0, 1.0, 0.0);
const MyFVector MyFVector::LeftVector(0.0, -1.0, 0.0);

MyFVector::MyFVector(double InX, double InY, double InZ)
    : X(InX), Y(InY), Z(InZ)
{
}

MyFVector& MyFVector::operator=(const MyFVector& Other)
{
    this->X = Other.X;
    this->Y = Other.Y;
    this->Z = Other.Z;


    return *this;
}

MyFVector MyFVector::operator^(const MyFVector& V) const
{
    return MyFVector
    (
        Y * V.Z - Z * V.Y,
        Z * V.X - X * V.Z,
        X * V.Y - Y * V.X
    );
}

MyFVector MyFVector::CrossProduct(const MyFVector& A, const MyFVector& B)
{
    return A ^ B;
}

double MyFVector::operator|(const MyFVector& V) const
{
    return X * V.X + Y * V.Y + Z * V.Z;
}

double MyFVector::DotProduct(const MyFVector& A, const MyFVector& B)
{
    return A | B;
}

MyFVector MyFVector::operator+(const MyFVector& V) const
{
    return MyFVector(X + V.X, Y + V.Y, Z + V.Z);
}

MyFVector MyFVector::operator-(const MyFVector& V) const
{
    return MyFVector(X - V.X, Y - V.Y, Z - V.Z);
}

MyFVector MyFVector::operator-(double Bias) const
{
    return MyFVector(X - Bias, Y - Bias, Z - Bias);
}

MyFVector MyFVector::operator+(double Bias) const
{
    return MyFVector(X + Bias, Y + Bias, Z + Bias);
}

MyFVector MyFVector::operator*(double Scale) const
{
    return MyFVector(X * Scale, Y * Scale, Z * Scale);
}

MyFVector MyFVector::operator/(double Scale) const
{
    const double RScale = 1.0 / Scale;
    return MyFVector(X * RScale, Y * RScale, Z * RScale);
}

MyFVector MyFVector::operator*(const MyFVector& V) const
{
    return MyFVector(X * V.X, Y * V.Y, Z * V.Z);
}

MyFVector MyFVector::operator/(const MyFVector& V) const
{
    return MyFVector(X / V.X, Y / V.Y, Z / V.Z);
}

bool MyFVector::operator==(const MyFVector& V) const
{
    return X == V.X && Y == V.Y && Z == V.Z;
}

bool MyFVector::operator!=(const MyFVector& V) const
{
    return X != V.X || Y != V.Y || Z != V.Z;
}

bool MyFVector::Equals(const MyFVector& V, double Tolerance) const
{
    return Abs(X - V.X) <= Tolerance && Abs(Y - V.Y) <= Tolerance &&
        Abs(Z - V.Z) <= Tolerance;
}


MyFVector MyFVector::operator-() const
{
    return MyFVector(-X, -Y, -Z);
}


MyFVector MyFVector::operator+=(const MyFVector& V)
{
    X += V.X;
    Y += V.Y;
    Z += V.Z;

    return *this;
}

MyFVector MyFVector::operator-=(const MyFVector& V)
{
    X -= V.X;
    Y -= V.Y;
    Z -= V.Z;

    return *this;
}

MyFVector MyFVector::operator*=(double Scale)
{
    X *= Scale;
    Y *= Scale;
    Z *= Scale;

    return *this;
}

MyFVector MyFVector::operator/=(double V)
{
    const double RV = 1.0 / V;
    X *= RV;
    Y *= RV;
    Z *= RV;

    return *this;
}

MyFVector MyFVector::operator*=(const MyFVector& V)
{
    X *= V.X;
    Y *= V.Y;
    Z *= V.Z;

    return *this;
}

MyFVector MyFVector::operator/=(const MyFVector& V)
{
    X /= V.X;
    Y /= V.Y;
    Z /= V.Z;

    return *this;
}

double& MyFVector::operator[](int Index)
{
    return (&X)[Index];
}

double MyFVector::operator[](int Index) const
{
    return (&X)[Index];
}

void MyFVector::Set(double InX, double InY, double InZ)
{
    X = InX;
    Y = InY;
    Z = InZ;
}

double MyFVector::GetMax() const
{
    return Max(Max(X, Y), Z);
}

double MyFVector::GetAbsMax() const
{
    return Max(Max(Abs(X), Abs(Y)), Abs(Z));
}

double MyFVector::GetMin() const
{
    return Min(Min(X, Y), Z);
}

double MyFVector::GetAbsMin() const
{
    return Min(Min(Abs(X), Abs(Y)), Abs(Z));
}

MyFVector MyFVector::GetAbs() const
{
    return MyFVector(Abs(X), Abs(Y), Abs(Z));
}

MyFVector MyFVector::GetSafeNormal(double Tolerance) const
{
    const double SquareSum = X * X + Y * Y + Z * Z;

    // Not sure if it's safe to add tolerance in there. Might introduce too many errors
    if (SquareSum == 1.0)
    {
        return *this;
    }
    else if (SquareSum < Tolerance)
    {
        return MyFVector::ZeroVector;
    }
    const double Scale = InvSqrt(SquareSum);
    return MyFVector(X * Scale, Y * Scale, Z * Scale);
}

MyFVector MyFVector::Projection() const
{
    const double RZ = 1.0 / Z;
    return MyFVector(X * RZ, Y * RZ, 1);
}

double MyFVector::Size() const
{
    return Sqrt(X * X + Y * Y + Z * Z);
}

double MyFVector::Dist(const MyFVector& V1, const MyFVector& V2)
{
    return Sqrt(MyFVector::DistSquared(V1, V2));
}

MyFVector MyFVector::FromFVector(const FVector& Other)
{
    return MyFVector(Other.X, Other.Y, Other.Z);
}

FVector MyFVector::ToFVector(const MyFVector& Other)
{
    return FVector(Other.X, Other.Y, Other.Z);
}

FVector MyFVector::UnrotateVector(const FQuat& Quat, const FVector& Vector)
{
    double Size=Vector.Size();
    FVector NormalVec=Vector.GetSafeNormal();
    FVector TargetVector = Quat.UnrotateVector(NormalVec);
    return Size*TargetVector;
}

double MyFVector::DistSquared(const MyFVector& V1, const MyFVector& V2)
{
    return Square(V2.X - V1.X) + Square(V2.Y - V1.Y) + Square(V2.Z - V1.Z);
}
