/*==============================================================================
UnArray.h: Dynamic array

Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

Description:
    This file defines a template class for handling dynamic arrays
    (arrays which can grow at run-time).
    Some of the functions are biased towards arrays where the order
    of the elements is not important.

Revision history:
    * 09/23/96: Created by Mark
==============================================================================*/

#ifndef _INC_UnArray
#define _INC_UnArray

template
<
    class ElementType   // The element type for the array.
,   int   InitialSize   // The initial size of the array.
,   int   SizeIncrement // For when the array is enlarged.
>
class DArray
{
public:
    DArray() { Clear(); }
    ~DArray() { Free(); }

    // Number of elements in the array:
    int Count() const { return State.Count; } 

    // Free any storage allocated for the array.
    void Free()
    {
        if( State.Elements != 0 )
        {
            delete[] State.Elements;
        }
        Clear();
    }

    // Empty the array, making its Count() zero, and optionally free the 
    // storage allocated for array elements.
    void Empty(BOOL FreeStorage = FALSE)
    {
        if( FreeStorage )
        {
            Free();
        }
        else
        {
            State.Count = 0;
        }
    }

    // Is the array empty?
    BOOL IsEmpty() const { return Count()==0; }

    // Set the number of elements in the array. (New elements have undefined values)
    void SetCount(int Count)
    {
        MakeRoom( Count );
        State.Count = Count;
    }

    // Increase the number of elements in the array. (New elements have undefined values)
    void IncreaseCount(int Change)
    {
        SetCount( Count() + Change );
    }

    // Make sure there is enough room in the array to hold Count items.
    // This does not change the current count of array items. You might 
    // want to use this for performance reasons (to avoid memory reallocations 
    // when filling in a lot of values), otherwise you can use SetCount to 
    // directly change the array size.
    void MakeRoom(int Count);

    // Indexing operations:
    ElementType & operator[](int N) { return State.Elements[N]; }
    const ElementType & operator[](int N) const { return State.Elements[N]; }

    // First() and Last() convenience functions:
    ElementType & First() { return State.Elements[0]; }
    const ElementType & First() const { return State.Elements[0]; }
    ElementType & Last() { return State.Elements[State.Count-1]; }
    const ElementType & Last() const { return State.Elements[State.Count-1]; }

    // Add a value to the end of the array:
    void Add(const ElementType & Value) 
    { 
        SetCount( Count()+1 ); 
        Last() = Value; 
    }

    // Add all the elements from Values to the end of *this.
    void Add(const DArray & Values) 
    { 
        MakeRoom( this->Count() + Values.Count() ); 
        for( int Which = 0; Which < Values.Count(); ++Which )
        {
            Add(Values[Which]);
        }
    }

    // Add an element to the end of the array, if the element is not already in the array.
    void AddUniquely(const ElementType & Value)
    {
        if( !Has(Value) )
        {
            Add(Value);
        }
    }

    // Add all the elements from Values (not already in *this) to the end of *this.
    void AddUniquely(const DArray & Values) 
    { 
        MakeRoom( this->Count() + Values.Count() ); 
        for( int Which = 0; Which < Values.Count(); ++Which )
        {
            AddUniquely(Values[Which]);
        }
    }

    // Remove the element at the given Index. 
    // This rearranges the array to reuse the vacated spot, so this function 
    // is not appropriate for arrays where the order is important.
    // Does nothing if Index is not in 0..Count()-1
    void RemoveIndex(int Index) 
    {
        if( Index < 0  )
        {
        }
        else if( Index < Count() )
        {
            // Use the vacated spot by filling it with the last element of the list.
            // This is harmless if we are actually removing the last element.
            State.Elements[Index] = Last();
            State.Count--;
        }
    }

    // Remove the first element in the array with the given value, if 
    // there is such an element.
    // This rearranges the array to reuse the vacated spot, so this function 
    // is not appropriate for arrays where the order is important.
    void RemoveElement(const ElementType & Value)
    {
        RemoveIndex( Find(Value) );
    }

    // Change the first element of the array with OldValue to NewValue, if such
    // an element is found.
    void ChangeElement(const ElementType & OldValue, const ElementType & NewValue)
    {
        const int Index = Find(OldValue);
        if( Index >= 0 )
        {
            (*this)[Index] = NewValue;
        }
    }

    // Remove all elements in the array with the given value, if there
    // are any such elements. This rearranges the array, so this function 
    // is not appropriate for arrays where the order is important.
    void RemoveElements(const ElementType & Value)
    {
        for( int Index = 0; Index < State.Count; ++Index )
        {
            if( State.Elements[Index] == Value )
            {
                // We found the value - replace it the the last element to reuse the vacated spot.
                State.Elements[Index] = State.Elements[State.Count-1];
                State.Count--;
                Index--; // To reiterate over the current index.
            }
        }
    }

    // Return the index of the first element of the array with given value.
    // Returns a negative value if not found.
    int Find(const ElementType & Value) const 
    {
        for( int Index = 0; Index < State.Count; ++Index )
        {
            if( State.Elements[Index] == Value )
            {
                return Index; // <==== Unstructured return
            }
        }
        return -1; // If we reach here, we didn't find it.
    }

    // Does the array have the specified value?
    BOOL Has(const ElementType & Value) const
    {
        return Find(Value) >= 0;
    }

    // Explicit assignment function (creates a copy of the given array):
    void CopyFrom(const DArray & Value)
    {
        this->Free();
        const int Count = Value.Count();
        this->SetCount( Count );
        for( int Which = 0; Which < Count; ++Which )
        {
            (*this)[Which] = Value[Which];
        }
    }

private:

    struct
    {
        int           Space     ; // Total space in the array (including used elements).
        int           Count     ; // Number of elements in (used portion of) Elements.
        ElementType * Elements  ; // Array elements. Note 1.
        // Notes:
        //    1. There are always this->Space elements in the array this->Elements,
        //       but only the first this->Count elements are defined.
    }
    State;
    void Clear()
    {
        State.Space     = 0 ;
        State.Count     = 0 ;
        State.Elements  = 0 ;
    }

};

template<class ElementType,int InitialSize, int SizeIncrement>
void DArray<ElementType,InitialSize,SizeIncrement>
::MakeRoom
(
    int Count
)
{
    const int MinimumAdditionalSpaceNeeded = Count - State.Space;
    if( MinimumAdditionalSpaceNeeded > 0 )
    {
        const int AdditionalSpace = 
            MinimumAdditionalSpaceNeeded > SizeIncrement
        ?   MinimumAdditionalSpaceNeeded
        :   SizeIncrement
        ;
        int NewSpace = State.Space + AdditionalSpace;
        if( State.Elements == 0 && NewSpace < InitialSize )
        {
            NewSpace = InitialSize;
        }
        ElementType * NewElements = new ElementType[NewSpace];
        if( State.Elements != 0 )
        {
            // Note: Use of memmove for performance limits generality.
            memmove( NewElements, State.Elements, State.Count*sizeof(ElementType) );
            delete [] State.Elements;
        }
        State.Elements  = NewElements   ;
        State.Space     = NewSpace      ;
    }
}

#endif
