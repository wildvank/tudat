/*    Copyright (c) 2010-2017, Delft University of Technology
 *    All rigths reserved
 *
 *    This file is part of the Tudat. Redistribution and use in source and
 *    binary forms, with or without modification, are permitted exclusively
 *    under the terms of the Modified BSD license. You should have received
 *    a copy of the license with this file. If not, please or visit:
 *    http://tudat.tudelft.nl/LICENSE.
 */

#ifndef TUDAT_BODY_H
#define TUDAT_BODY_H

#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <Eigen/Core>

#include <Tudat/Astrodynamics/Aerodynamics/atmosphereModel.h>
#include <Tudat/Astrodynamics/Aerodynamics/aerodynamicCoefficientInterface.h>
#include <Tudat/Astrodynamics/Aerodynamics/flightConditions.h>
#include <Tudat/Astrodynamics/BasicAstrodynamics/timeConversions.h>
#include <Tudat/Astrodynamics/BasicAstrodynamics/accelerationModel.h>
#include <Tudat/Astrodynamics/BasicAstrodynamics/bodyShapeModel.h>
#include <Tudat/Astrodynamics/Ephemerides/ephemeris.h>
#include <Tudat/Astrodynamics/Ephemerides/rotationalEphemeris.h>
#include <Tudat/Astrodynamics/Gravitation/gravityFieldModel.h>
#include <Tudat/Astrodynamics/Gravitation/gravityFieldVariations.h>
#include <Tudat/Astrodynamics/Gravitation/timeDependentSphericalHarmonicsGravityField.h>
#include <Tudat/Astrodynamics/GroundStations/groundStation.h>
#include <Tudat/Astrodynamics/ElectroMagnetism/radiationPressureInterface.h>
#include <Tudat/Astrodynamics/ReferenceFrames/dependentOrientationCalculator.h>
#include <Tudat/Basics/basicTypedefs.h>
#include <Tudat/Astrodynamics/Ephemerides/rotationalEphemeris.h>
#include <Tudat/Astrodynamics/SystemModels/vehicleSystems.h>

namespace tudat
{

namespace simulation_setup
{

//! Base class used for the determination of the inertial state of a Body's ephemeris origin
/*!
 *  Base class used for the determination of the inertial state of a Body's ephemeris origin. This base class is used
 *  to provide an untemplated interface class through which to call the base frame state. The state may be defined
 *  in a templated manner in the derived class.
 */
class BaseStateInterface
{
public:

    //! Constructor
    /*!
     * Constructor
     * \param baseFrameId Name of frame origin for which inertial state is computed by this class
     */
    BaseStateInterface(
            const std::string baseFrameId ):
        baseFrameId_( baseFrameId ){ }

    //! Destructor
    virtual ~BaseStateInterface( ){ }

    //! Function through which the state of baseFrameId_ in the inertial frame can be determined
    /*!
     *  Function through which the state of baseFrameId_ in the inertial frame can be determined
     *  \param time Time at which state is to be computed
     *  \return Inertial state of frame origin at requested time
     */
    template< typename OutputTimeType, typename OutputStateScalarType >
    Eigen::Matrix< OutputStateScalarType, 6, 1 > getBaseFrameState(
            const OutputTimeType time );

protected:

    //! Pure virtual function through which the state of baseFrameId_ in the inertial frame can be determined
    /*!
     *  Pure virtual function through which the state of baseFrameId_ in the inertial frame can be determined
     *  (double time and double state scalar).
     *  \param time Time at which state is to be computed
     *  \return Inertial state of frame origin at requested time
     */
    virtual Eigen::Matrix< double, 6, 1 > getBaseFrameDoubleState( const double time ) = 0;

    //! Pure virtual function through which the state of baseFrameId_ in the inertial frame can be determined
    /*!
     *  Pure virtual function through which the state of baseFrameId_ in the inertial frame can be determined
     *  (double time and double long state scalar).
     *  \param time Time at which state is to be computed
     *  \return Inertial state of frame origin at requested time
     */
    virtual Eigen::Matrix< long double, 6, 1 > getBaseFrameLongDoubleState( const double time ) = 0;

    //! Pure virtual function through which the state of baseFrameId_ in the inertial frame can be determined
    /*!
     *  Pure virtual function through which the state of baseFrameId_ in the inertial frame can be determined
     *  (Time object time and double state scalar).
     *  \param time Time at which state is to be computed
     *  \return Inertial state of frame origin at requested time
     */
    virtual Eigen::Matrix< double, 6, 1 > getBaseFrameDoubleState( const Time& time ) = 0;

    //! Pure virtual function through which the state of baseFrameId_ in the inertial frame can be determined
    /*!
     *  Pure virtual function through which the state of baseFrameId_ in the inertial frame can be determined
     *  (Time object time and long double state scalar).
     *  \param time Time at which state is to be computed
     *  \return Inertial state of frame origin at requested time
     */
    virtual Eigen::Matrix< long double, 6, 1 > getBaseFrameLongDoubleState( const Time& time ) = 0;

    //! Name of frame origin for which inertial state is computed by this class
    std::string baseFrameId_;
};

//! Class used for the determination of the inertial state of a Body's ephemeris origin
template< typename TimeType, typename StateScalarType >
class BaseStateInterfaceImplementation: public BaseStateInterface
{
public:

    //! Constructor
    /*!
     * Constructor
     * \param baseFrameId Name of frame origin for which inertial state is computed by this class
     * \param stateFunction Function returning frame's inertial state as a function of time.
     */
    BaseStateInterfaceImplementation(
            const std::string baseFrameId,
            const boost::function< Eigen::Matrix< StateScalarType, 6, 1 >( const TimeType ) > stateFunction ):
        BaseStateInterface( baseFrameId ),
        stateFunction_( stateFunction ){ }

    //! Destructor
    ~BaseStateInterfaceImplementation( ){ }


protected:

    //! Function through which the state of baseFrameId_ in the inertial frame can be determined
    /*!
     *  Function through which the state of baseFrameId_ in the inertial frame can be determined
     *  (double time and double state scalar).
     *  \param time Time at which state is to be computed
     *  \return Inertial state of frame origin at requested time
     */
    Eigen::Matrix< double, 6, 1 > getBaseFrameDoubleState( const double time )
    {
        return stateFunction_( time ).template cast< double >( );
    }

    //! Function through which the state of baseFrameId_ in the inertial frame can be determined
    /*!
     *  Function through which the state of baseFrameId_ in the inertial frame can be determined
     *  (double time and double long state scalar).
     *  \param time Time at which state is to be computed
     *  \return Inertial state of frame origin at requested time
     */
    Eigen::Matrix< long double, 6, 1 > getBaseFrameLongDoubleState( const double time )
    {
        return stateFunction_( time ).template cast< long double >( );
    }

    //! Function through which the state of baseFrameId_ in the inertial frame can be determined
    /*!
     *  Function through which the state of baseFrameId_ in the inertial frame can be determined
     *  (Time object time and double state scalar).
     *  \param time Time at which state is to be computed
     *  \return Inertial state of frame origin at requested time
     */
    Eigen::Matrix< double, 6, 1 > getBaseFrameDoubleState( const Time& time )
    {
        return stateFunction_( time ).template cast< double >( );
    }

    //! Function through which the state of baseFrameId_ in the inertial frame can be determined
    /*!
     *  Function through which the state of baseFrameId_ in the inertial frame can be determined
     *  (Time object time and long double state scalar).
     *  \param time Time at which state is to be computed
     *  \return Inertial state of frame origin at requested time
     */
    Eigen::Matrix< long double, 6, 1 > getBaseFrameLongDoubleState( const Time& time )
    {
        return stateFunction_( time ).template cast< long double >( );
    }

private:

    //! Function returning frame's inertial state as a function of time.
    boost::function< Eigen::Matrix< StateScalarType, 6, 1 >( const TimeType ) > stateFunction_;
};

//! Body class representing the properties of a celestial body (natural or artificial).
/*!
 *  Body class representing the properties of a celestial body (natural or artificial). By storing
 *  all properties of bodies (ephemeris, rotation, gravity, etc.) in a set of body objects,
 *  the simulation environment can be defined in a clear and modular way. To create body
 *  objects, the createBodies.h function provides a range of functionality. The
 *  createAccelerationModels.h file provides functions to use body objects to create acceleration
 *  objects.
 */
class Body
{
public:

    //! Constructor for a body
    /*!
     * Constructor for a body, sets current state (with zero default value).
     * \param state Current state of body at initialization (default = zeroes).
     */
    Body( const Eigen::Vector6d& state =
            Eigen::Vector6d::Zero( ) )
        : currentState_( state ), timeOfCurrentState_( TUDAT_NAN ),
          ephemerisFrameToBaseFrame_( boost::make_shared< BaseStateInterfaceImplementation< double, double > >(
                                          "", boost::lambda::constant( Eigen::Vector6d::Zero( ) ) ) ),
          currentRotationToLocalFrame_( Eigen::Quaterniond( Eigen::Matrix3d::Identity( ) ) ),
          currentRotationToLocalFrameDerivative_( Eigen::Matrix3d::Zero( ) ),
          currentAngularVelocityVectorInGlobalFrame_( Eigen::Vector3d::Zero( ) ),
          bodyMassFunction_( NULL )
    {
        currentLongState_ = currentState_.cast< long double >( );
    }

    //! Function to retrieve the class returning the state of this body's ephemeris origin w.r.t. the global origin
    /*!
     * Function to retrieve the class returning the state of this body's ephemeris origin w.r.t. the global origin
     * \return Class returning the state of this body's ephemeris origin w.r.t. the global origin
     */
    boost::shared_ptr< BaseStateInterface > getEphemerisFrameToBaseFrame( )
    {
        return ephemerisFrameToBaseFrame_;
    }

    //! Function to set the class returning the state of this body's ephemeris origin w.r.t. the global origin
    /*!
     * Function to set the class returning the state of this body's ephemeris origin w.r.t. the global origin
     * \param ephemerisFrameToBaseFrame Class returning the state of this body's ephemeris origin w.r.t. the global origin
     */
    void setEphemerisFrameToBaseFrame( const boost::shared_ptr< BaseStateInterface > ephemerisFrameToBaseFrame )
    {
        ephemerisFrameToBaseFrame_ = ephemerisFrameToBaseFrame;
    }


    //! Set current state of body manually
    /*!
     * Set current state of body manually, which must be in the global frame. Note that this
     * function does not set the currentLongState_, use the setLongState when needing the use of the
     * long precision current state.
     * \param state Current state of the body that is set.
     */
    void setState( const Eigen::Vector6d& state )
    {
        currentState_ = state;
    }

    //! Set current state of body manually in long double precision.
    /*!
     * Set current state of body manually in long double precision. State must be in the global
     * frame.  Note that this function sets both the currentState_ and currentLongState_ variables
     * (currentLongState_ directly and currentState_ by casting the input to double entries).
     * \param longState Current state of the body that is set, in long double precision.
     */
    void setLongState( const Eigen::Matrix< long double, 6, 1 >& longState )
    {
        currentLongState_ = longState;
        currentState_ = longState.cast< double >( );
    }

    //! Templated function to set the state manually.
    /*!
     * Templated function to set the state manually, calls either setState or setLongState function.
     * \param state Current state of the body that is set, with StateScalarType precision.
     */
    template< typename StateScalarType >
    void setTemplatedState( const Eigen::Matrix< StateScalarType, 6, 1 >& state );

    //! Templated function to set the current state of the body from its ephemeris and
    //! global-to-ephemeris-frame function.
    /*!
     * Templated function to set the current state of the body from its ephemeris and
     * global-to-ephemeris-frame function. It sets both the currentState_ and currentLongState_ variables. F
     * FUndamental coputation is done on state with StateScalarType precision as a function of TimeType time
     * \param time Time at which the global state is to be set.
     */
    template< typename StateScalarType = double, typename TimeType = double >
    void setStateFromEphemeris( const TimeType& time )
    {
        if( !( static_cast< Time >( time ) == timeOfCurrentState_ ) )
        {
            if( sizeof( StateScalarType ) == 8 )
            {
                currentState_ =
                        ( bodyEphemeris_->getTemplatedStateFromEphemeris< StateScalarType, TimeType >( time ) +
                          ephemerisFrameToBaseFrame_->getBaseFrameState< TimeType, StateScalarType >( time ) ).
                        template cast< double >( );
                currentLongState_ = currentState_.template cast< long double >( );
            }
            else
            {
                currentLongState_ =
                        ( bodyEphemeris_->getTemplatedStateFromEphemeris< StateScalarType, TimeType >( time ) +
                          ephemerisFrameToBaseFrame_->getBaseFrameState< TimeType, StateScalarType >( time ) ).
                        template cast< long double >( );
                currentState_ = currentLongState_.template cast< double >( );
            }

            timeOfCurrentState_ = static_cast< TimeType >( time );
        }
    }

    //! Templated function to get the current state of the body from its ephemeris and
    //! global-to-ephemeris-frame function.
    /*!
     * Templated function to get the current state of the body from its ephemeris and
     * global-to-ephemeris-frame function.  It calls the setStateFromEphemeris state, resetting the currentState_ /
     * currentLongState_ variables, and returning the state with the requested precision
     * \return State at requested time
     */
    template< typename StateScalarType = double, typename TimeType = double >
    Eigen::Matrix< StateScalarType, 6, 1 > getStateInBaseFrameFromEphemeris( const TimeType time )
    {
       setStateFromEphemeris< StateScalarType, TimeType >( time );
       if( sizeof( StateScalarType ) == 8 )
       {
           return currentState_.template cast< StateScalarType >( );
       }
       else
       {
           return currentLongState_.template cast< StateScalarType >( );
       }

    }

    //! Get current state.
    /*!
     * Returns the internally stored current state vector.
     * \return Current state.
     */
    Eigen::Vector6d getState( ) { return currentState_; }

    //! Get current position.
    /*!
     * Returns the internally stored current position vector.
     * \return Current position.
     */
    Eigen::Vector3d getPosition( ) { return currentState_.segment( 0, 3 ); }

    //! Get current velocity.
    /*!
     * Returns the internally stored current velocity vector.
     * \return Current velocity.
     */
    Eigen::Vector3d getVelocity( ) { return currentState_.segment( 3, 3 ); }


    //! Get current state, in long double precision
    /*!
     * Returns the internally stored current state vector, in long double precision
     * \return Current state, in long double precisio
     */
    Eigen::Matrix< long double, 6, 1 > getLongState( ) { return currentLongState_; }

    //! Get current position, in long double precision
    /*!
     * Returns the internally stored current position vector, in long double precision
     * \return Current position, in long double precision
     */
    Eigen::Matrix< long double, 3, 1 > getLongPosition( ) { return currentLongState_.segment( 0, 3 ); }

    //! Get current velocity, in long double precision.
    /*!
     * Returns the internally stored current velocity vector.
     * \return Current velocity, in long double precision
     */
    Eigen::Matrix< long double, 3, 1 > getLongVelocity( ) { return currentLongState_.segment( 3, 3 ); }

    //! Templated function to retrieve the state.
    /*!
     * Templated function to retrieve the state, calls either getState or getLongState function.
     * \return  Current state of the body, with StateScalarType precision.
     */
    template< typename ScalarStateType >
    Eigen::Matrix< ScalarStateType, 6, 1 > getTemplatedState( );

    //! Function to set the rotation from global to body-fixed frame at given time
    /*!
     * Function to set the rotation from global to body-fixed frame at given time, using the
     * rotationalEphemeris_ member object
     * \param time Time at which the rotation is to be retrieved.
     */
    void setCurrentRotationToLocalFrameFromEphemeris( const double time )
    {
        if( rotationalEphemeris_!= NULL )
        {
            currentRotationToLocalFrame_ = rotationalEphemeris_->getRotationToTargetFrame( time );
        }
        else if( dependentOrientationCalculator_ != NULL )
        {
            currentRotationToLocalFrame_ = dependentOrientationCalculator_->getRotationToLocalFrame( time );
        }
        else
        {
            throw std::runtime_error(
                        "Error, no rotation model found in Body::setCurrentRotationToLocalFrameFromEphemeris" );
        }
    }

    //! Function to set the rotation matrix derivative from global to body-fixed frame at given time
    /*!
     * Function to set the rotation matrix derivative from global to body-fixed frame at given time,
     * using the rotationalEphemeris_ member object
     * \param time Time at which the rotation matrix derivative is to be retrieved.
     */
    void setCurrentRotationToLocalFrameDerivativeFromEphemeris( const double time )
    {
        if( rotationalEphemeris_!= NULL )
        {
            currentRotationToLocalFrameDerivative_
                    = rotationalEphemeris_->getDerivativeOfRotationToTargetFrame( time );
        }
        else if( dependentOrientationCalculator_ != NULL )
        {
            currentRotationToLocalFrameDerivative_.setZero( );
        }
        else
        {
            throw std::runtime_error(
                   "Error, no rotationalEphemeris_ found in Body::setCurrentRotationToLocalFrameDerivativeFromEphemeris" );
        }
    }

    //! Function to set the angular velocity vector in the global frame at given time
    /*!
     * Function to set the angular velocity vector in the global frame at given time, using the
     * rotationalEphemeris_ member object
     * \param time Time at which the angular velocity vector in the global frame is to be retrieved.
     */
    void setCurrentAngularVelocityVectorInGlobalFrame( const double time )
    {
        if( rotationalEphemeris_!= NULL )
        {
            currentAngularVelocityVectorInGlobalFrame_
                    = rotationalEphemeris_->getRotationalVelocityVectorInBaseFrame( time );
        }
        else if( dependentOrientationCalculator_ != NULL )
        {
            currentAngularVelocityVectorInGlobalFrame_.setZero( );
        }
        else
        {
            throw std::runtime_error(
                        "Error, no rotationalEphemeris_ found in Body::setCurrentAngularVelocityVectorInGlobalFrame" );
        }
    }

    //! Function to set the full rotational state at given time
    /*!
     * Function to set the full rotational state at (rotation from global to body-fixed frame
     * rotation matrix derivative from global to body-fixed frame and angular velocity vector in the
     * global frame) at given time, using the rotationalEphemeris_ member object.
     * \param time Time at which the angular velocity vector in the global frame is to be retrieved.
     */
    void setCurrentRotationalStateToLocalFrameFromEphemeris( const double time )
    {
        if( rotationalEphemeris_!= NULL )
        {
            rotationalEphemeris_->getFullRotationalQuantitiesToTargetFrame(
                        currentRotationToLocalFrame_, currentRotationToLocalFrameDerivative_,
                        currentAngularVelocityVectorInGlobalFrame_, time );
        }
        else if( dependentOrientationCalculator_ != NULL )
        {
            currentRotationToLocalFrame_ = dependentOrientationCalculator_->getRotationToLocalFrame( time );
            currentRotationToLocalFrameDerivative_.setZero( );
            currentAngularVelocityVectorInGlobalFrame_.setZero( );
        }
        else
        {
            throw std::runtime_error(
                        "Error, no rotationalEphemeris_ found in Body::setCurrentRotationalStateToLocalFrameFromEphemeris" );
        }
    }


    //! Get current rotation from body-fixed to inertial frame.
    /*!
     *  Get current rotation from body-fixed to inertial frame, as set from the rotationalEphemeris_
     *  by the setCurrentRotationalStateToLocalFrameFromEphemeris or
     *  setCurrentRotationToLocalFrameFromEphemeris function.  If body has no rotational ephemeris,
     *  an identity quaternion (no rotation) is returned.
     *  \return Current rotation from body-fixed to inertial frame
     */
    Eigen::Quaterniond getCurrentRotationToGlobalFrame( )
    {
        return currentRotationToLocalFrame_.inverse( );
    }

    //! Get current rotation from inertial to body-fixed frame.
    /*!
     *  Get current rotation from inertial to body-fixed frame, as set from the rotationalEphemeris_
     *  by the setCurrentRotationalStateToLocalFrameFromEphemeris or
     *  setCurrentRotationToLocalFrameFromEphemeris function.  If body has no rotational ephemeris,
     *  an identity quaternion (no rotation) is returned.
     *  \return Current rotation from inertial to body-fixed frame
     */
    Eigen::Quaterniond getCurrentRotationToLocalFrame( )
    {
        return currentRotationToLocalFrame_;
    }

    //! Get current rotation matrix derivative from body-fixed to global frame.
    /*!
     *  Get current rotation matrix derivative from body-fixed frame to global, as set from the
     *  rotationalEphemeris_ by the setCurrentRotationalStateToLocalFrameFromEphemeris or
     *  setCurrentRotationToLocalFrameDerivativeFromEphemeris function. If body has no rotational
     *  ephemeris, an zero matrix (no rotation) is returned.
     *  \return Current otation matrix derivative from global to body-fixed frame.
     */
    Eigen::Matrix3d getCurrentRotationMatrixDerivativeToGlobalFrame( )
    {
        return currentRotationToLocalFrameDerivative_.transpose( );
    }

    //! Get current rotation matrix derivative from global to body-fixed frame.
    /*!
     *  Get current rotation matrix derivative from global to body-fixed frame, as set from the
     *  rotationalEphemeris_ by the setCurrentRotationalStateToLocalFrameFromEphemeris or
     *  setCurrentRotationToLocalFrameDerivativeFromEphemeris function. If body has no rotational
     *  ephemeris, an zero matrix (no rotation) is returned.
     *  \return Current otation matrix derivative from global to body-fixed frame.
     */
    Eigen::Matrix3d getCurrentRotationMatrixDerivativeToLocalFrame( )
    {
        return currentRotationToLocalFrameDerivative_;
    }

    //! Function to set the ephemeris of the body.
    /*!
     *  Function to set the ephemeris of the body, which is used to represent the (a priori)
     *  state history of the body.
     *  \param bodyEphemeris New ephemeris of the body.
     */
    void setEphemeris( const boost::shared_ptr< ephemerides::Ephemeris > bodyEphemeris )
    {
        bodyEphemeris_ = bodyEphemeris;
    }

    //! Function to set the gravity field of the body.
    /*!
     *  Function to set the gravity field of the body; input is also used to (re)set the mass of the
     *  body.
     *  \param gravityFieldModel New gravity field of the body.
     */
    void setGravityFieldModel(
            const boost::shared_ptr< gravitation::GravityFieldModel > gravityFieldModel )
    {
        gravityFieldModel_ = gravityFieldModel;

        // Update current mass of body, provide warning
        if( bodyMassFunction_ != NULL )
        {
            std::cerr<<"Warning when settings gravity field model for body, mass function already found: resetting"<<std::endl;
        }

        currentMass_ = gravityFieldModel_->getGravitationalParameter( )
                       / physical_constants::GRAVITATIONAL_CONSTANT;
        bodyMassFunction_ = boost::lambda::constant( currentMass_ );
    }

    //! Function to set the atmosphere model of the body.
    /*!
     *  Function to set the atmosphere model of the body.
     *  \param atmosphereModel Atmosphere model of the body.
     */
    void setAtmosphereModel(
            const boost::shared_ptr< aerodynamics::AtmosphereModel > atmosphereModel )
    {
        atmosphereModel_ = atmosphereModel;
    }

    //! Function to set the rotation model of the body.
    /*!
     *  Function to set the rotation model of the body.
     *  \param rotationalEphemeris Rotation model of the body.
     */
    void setRotationalEphemeris(
            const boost::shared_ptr< ephemerides::RotationalEphemeris > rotationalEphemeris )
    {
        if( dependentOrientationCalculator_ != NULL )
        {
            std::cerr<<"Warning when setting rotational ephemeris, dependentOrientationCalculator_ already found, NOT setting closure"<<std::endl;
        }
        rotationalEphemeris_ = rotationalEphemeris;
    }

    //! Function to set a rotation model that is only valid during numerical propagation
    /*!
     *  Function to set a rotation model that is only valid during numerical propagation, as it depends on the full state
     *  of the environment
     *  \param dependentOrientationCalculator Object from which the orientation is computed.
     */
    void setDependentOrientationCalculator(
            const boost::shared_ptr< reference_frames::DependentOrientationCalculator > dependentOrientationCalculator )
    {
        // Check if object already exists
        if( dependentOrientationCalculator_ != NULL )
        {
            // Try to create closure between new and existing objects (i.e ensure that they end up computing the same rotation
            // in differen manenrs.
            if( ( boost::dynamic_pointer_cast< reference_frames::AerodynamicAngleCalculator >(
                      dependentOrientationCalculator ) != NULL ) &&
                    ( boost::dynamic_pointer_cast< reference_frames::AerodynamicAngleCalculator >(
                          dependentOrientationCalculator_ ) == NULL ) )
            {
                reference_frames::setAerodynamicDependentOrientationCalculatorClosure(
                            dependentOrientationCalculator_,
                            boost::dynamic_pointer_cast< reference_frames::AerodynamicAngleCalculator >(
                                dependentOrientationCalculator ) );
            }
            else if( ( boost::dynamic_pointer_cast< reference_frames::AerodynamicAngleCalculator >(
                           dependentOrientationCalculator_ ) != NULL ) &&
                     ( boost::dynamic_pointer_cast< reference_frames::AerodynamicAngleCalculator >(
                           dependentOrientationCalculator ) == NULL ) )
            {
                reference_frames::setAerodynamicDependentOrientationCalculatorClosure(
                            dependentOrientationCalculator,
                            boost::dynamic_pointer_cast< reference_frames::AerodynamicAngleCalculator >(
                                dependentOrientationCalculator_ ) );
            }
            else
            {
                std::cerr<< "Warning, cannot reset dependentOrientationCalculator, incompatible object already exists" <<std::endl;
            }
        }
        else
        {
            dependentOrientationCalculator_ = dependentOrientationCalculator;
        }
    }

    //! Function to set the shape model of the body.
    /*!
     *  Function to set the shape model of the body.
     *  \param shapeModel Shape model of the body.
     */
    void setShapeModel( const boost::shared_ptr< basic_astrodynamics::BodyShapeModel > shapeModel )
    {
        shapeModel_ = shapeModel;
    }

    //! Function to set the aerodynamic coefficient interface of the body.
    /*!
     *  Function to set the aerodynamic coefficient interface of the body.
     *  \param aerodynamicCoefficientInterface Aerodynamic coefficient interface of the body.
     */
    void setAerodynamicCoefficientInterface(
            const boost::shared_ptr< aerodynamics::AerodynamicCoefficientInterface >
            aerodynamicCoefficientInterface)
    {
        aerodynamicCoefficientInterface_ = aerodynamicCoefficientInterface;
    }

    //! Function to set the body flight conditions
    /*!
     * Function to set the body flight conditions, which calculates current aerodynamic angles,
     * altitude, etc.
     * \param aerodynamicFlightConditions Body flight conditions
     */
    void setFlightConditions(
            const boost::shared_ptr< aerodynamics::FlightConditions > aerodynamicFlightConditions )
    {
        aerodynamicFlightConditions_ = aerodynamicFlightConditions;

        // If dependentOrientationCalculator_ object already exists, provide a warning and create closure between the two
        if( dependentOrientationCalculator_ != NULL )
        {
            reference_frames::setAerodynamicDependentOrientationCalculatorClosure(
                        dependentOrientationCalculator_, aerodynamicFlightConditions_->getAerodynamicAngleCalculator( ) );
        }
        else
        {
            dependentOrientationCalculator_ = aerodynamicFlightConditions->getAerodynamicAngleCalculator( );
        }

        // Create closure between rotational ephemeris and aerodynamic angle calculator.
        if( rotationalEphemeris_ != NULL )
        {
            reference_frames::setAerodynamicDependentOrientationCalculatorClosure(
                        rotationalEphemeris_, aerodynamicFlightConditions_->getAerodynamicAngleCalculator( )  );
        }
    }

    //! Function to set the radiation pressure interface of the body, for a single radiation source.
    /*!
     *  Function to set the radiation pressure interface of the body, for a single radiation source
     *  \param radiatingBody Name of body that is the source of the radiation.
     *  \param radiationPressureInterface Radiation pressure interface of the body.
     */
    void setRadiationPressureInterface(
            const std::string& radiatingBody,
            const boost::shared_ptr< electro_magnetism::RadiationPressureInterface >
                radiationPressureInterface )
    {
        radiationPressureInterfaces_[ radiatingBody ] = radiationPressureInterface;
    }

    //! Function to set object containing all variations in the gravity field of this body.
    /*!
     * Function to set object containing all variations in the gravity field of this body.
     * \param gravityFieldVariationSet Object containing all variations in the gravity field of this body.
     */
    void setGravityFieldVariationSet(
            const boost::shared_ptr< gravitation::GravityFieldVariationsSet >
                gravityFieldVariationSet )
    {
        gravityFieldVariationSet_ = gravityFieldVariationSet;
    }

    //! Function to get the gravity field model of the body.
    /*!
     *  Function to get the gravity field model of the body.
     *  \return Gravity field model of the body.
     */
    boost::shared_ptr< gravitation::GravityFieldModel > getGravityFieldModel( )
    {
        return gravityFieldModel_;
    }

    //! Function to get the ephemeris of the body.
    /*!
     *  Function to get the ephemeris of the body.
     *  \return Ephemeris of the body.
     */
    boost::shared_ptr< ephemerides::Ephemeris > getEphemeris( )
    {
        return bodyEphemeris_;
    }

    //! Function to get the atmosphere model of the body.
    /*!
     *  Function to get the atmosphere model of the body.
     *  \return Atmosphere model of the body.
     */
    boost::shared_ptr< aerodynamics::AtmosphereModel > getAtmosphereModel( )
    {
        return atmosphereModel_;
    }

    //! Function to get the rotation model of the body.
    /*!
     *  Function to get the rotation model of the body.
     *  \return Rotation model of the body.
     */
    boost::shared_ptr< ephemerides::RotationalEphemeris > getRotationalEphemeris( )
    {
        return rotationalEphemeris_;
    }

    //! Function to retrieve the model to compute the rotation of the body based on the current state of the environment.
    /*!
     * Function to retrieve the model to compute the rotation of the body based on the current state of the environment
     * (model is only valid during propagation).
     * \return Model to compute the rotation of the body based on the current state of the environment
     */
    boost::shared_ptr< reference_frames::DependentOrientationCalculator > getDependentOrientationCalculator( )
    {
        return dependentOrientationCalculator_;
    }

    //! Function to retrieve the shape model of body.
    /*!
     * Function to retrieve the shape model of body.
     * \return Shape model of body.
     */
    boost::shared_ptr< basic_astrodynamics::BodyShapeModel > getShapeModel( )
    {
        return shapeModel_;
    }

    //! Function to retrieve the aerodynamic coefficient model of body.
    /*!
     * Function to retrieve the body aerodynamic coefficient model of body.
     * \return Aerodynamic coefficient model of body.
     */
    boost::shared_ptr< aerodynamics::AerodynamicCoefficientInterface >
    getAerodynamicCoefficientInterface( )
    {
        return aerodynamicCoefficientInterface_;
    }

    //! Function to retrieve the body flight conditions
    /*!
     * Function to retrieve the body flight conditions, which calculates current aerodynamic angles,
     * altitude, etc.
     * \return Body flight conditions
     */
    boost::shared_ptr< aerodynamics::FlightConditions > getFlightConditions( )
    {
        return aerodynamicFlightConditions_;
    }

    //! Function to retrieve the shape model of the body.
    /*!
     *  Function to retrieve the shape model of the body.
     *  \return Shape model of the body.
     */
    std::map< std::string, boost::shared_ptr< electro_magnetism::RadiationPressureInterface > >
    getRadiationPressureInterfaces( )
    {
        return radiationPressureInterfaces_;
    }

    //! Function to retrieve a single object describing variation in the gravity field of this body.
    /*!
     *  Function to retrieve a single object describing variation in the gravity field of this body.
     *  \param deformationType Type of gravity field variation.
     *  \param identifier Identifier of gravity field variation that is to be retrieved (empty by default; only required
     *  if multiple variations of same type are present)
     *  \return Object describing requested variation in the gravity field of this body.
     */
    std::pair< bool, boost::shared_ptr< gravitation::GravityFieldVariations > >
            getGravityFieldVariation(
                const gravitation::BodyDeformationTypes& deformationType,
                const std::string identifier = "" )
    {
        return gravityFieldVariationSet_->getGravityFieldVariation( deformationType, identifier );
    }

    //! Function to retrieve object containing all variations in the gravity field of this body.
    /*!
     * Function to retrieve object containing all variations in the gravity field of this body.
     * \return Object containing all variations in the gravity field of this body.
     */
    boost::shared_ptr< gravitation::GravityFieldVariationsSet > getGravityFieldVariationSet( )
    {
        return gravityFieldVariationSet_;
    }

    //! Function to retrieve container object with hardware systems present on/in body
    /*!
     * Function to retrieve container object with hardware systems present on/in body.
     * \return Container object with hardware systems present on/in body
     */
    boost::shared_ptr< system_models::VehicleSystems > getVehicleSystems( )
    {
        return vehicleSystems_;
    }

    //! Function to set container object with hardware systems present on/in body
    /*!
     * Function to set container object with hardware systems present on/in body (typically only non-NULL for a vehicle).
     * \param vehicleSystems Container object with hardware systems present on/in body
     */
    void setVehicleSystems( const boost::shared_ptr< system_models::VehicleSystems > vehicleSystems )
    {
        vehicleSystems_ = vehicleSystems;
    }

    //! Function to set the function returning body mass as a function of time
    /*!
     * Function to set the function returning body mass as a function of time
     * \param bodyMassFunction Function returning body mass as a function of time
     */
    void setBodyMassFunction( const boost::function< double( const double ) > bodyMassFunction )
    {
        bodyMassFunction_ = bodyMassFunction;
    }

    //! Function to set the body mass as being constant (i.e. time-independent)
    /*!
     * Function to set the body mass as being constant (i.e. time-independent)
     * \param bodyMass New constant body mass
     */
    void setConstantBodyMass( const double bodyMass )
    {
        bodyMassFunction_ = boost::lambda::constant( bodyMass );
        currentMass_ = bodyMass;
    }

    //! Function to get the function returning body mass as a function of time
    /*!
     * Function to get the function returning body mass as a function of time
     * \return Function returning body mass as a function of time
     */
    boost::function< double( const double ) > getBodyMassFunction( )
    {
        return bodyMassFunction_;
    }

    //! Function to update the body mass to the current time
    /*!
     * Function to update the body mass to the current time, using the bodyMassFunction_ function
     * \param time Current time
     */
    void updateMass( const double time )
    {
        if( bodyMassFunction_ != NULL )
        {
            currentMass_ = bodyMassFunction_( time );
        }
        else
        {
            throw std::runtime_error( "Error when updating body mass, no mass function is set" );
        }
    }

    //! Function to retrieve the current body mass
    /*!
     * Function to retrieve the current body mass
     * \return Current body mass.
     */
    double getBodyMass( )
    {
        return currentMass_;
    }

    //! Function to add a ground station to the body
    /*!
     * Function to add a ground station to the body
     * \param stationName Name of ground station
     * \param station Ground station object that is to be set
     */
    void addGroundStation( const std::string& stationName,
                           const boost::shared_ptr< ground_stations::GroundStation >& station )
    {
        groundStationMap[ stationName ] = station;
    }

    //! Function to retrieve a ground station
    /*!
     * Function to retrieve a ground station
     * \param stationName Name of ground station
     * \return Ground station object that is retrieved
     */
    boost::shared_ptr< ground_stations::GroundStation > getGroundStation( const std::string& stationName ) const
    {
        if( groundStationMap.count( stationName ) == 0 )
        {
            std::cerr<<"Warning, station "<<stationName<<" does not exist"<<std::endl;
        }

        return groundStationMap.at( stationName );
    }

    //! Function to retrieve full list of ground stations
    /*!
     * Function to retrieve full list of ground stations
     * \return Full list of ground stations
     */
    std::map< std::string, boost::shared_ptr< ground_stations::GroundStation > > getGroundStationMap( ) const
    {
        return groundStationMap;
    }

    //! Function to recompute the internal variables of member variables that depend on the ephemerides bodies.
    /*!
     * Function to recompute the internal variables of member variables that depend on the ephemerides of this and other
     * bodies. This function is typically called after equations of motion have been computed and set in environment to
     * ensure full model consistency.
     */
    void updateConstantEphemerisDependentMemberQuantities( )
    {
        if( boost::dynamic_pointer_cast< gravitation::TimeDependentSphericalHarmonicsGravityField >(
                    gravityFieldModel_ ) != NULL )
        {
            boost::dynamic_pointer_cast< gravitation::TimeDependentSphericalHarmonicsGravityField >(
                        gravityFieldModel_ )->updateCorrectionFunctions( );
        }
    }

    //! Function to indicate that the state needs to be recomputed on next call to setStateFromEphemeris.
    /*!
     * Function to reset the time to which the state was last updated using setStateFromEphemeris function to nan, thereby
     * singalling that it needs to be recomputed upon next call.
     */
    void recomputeStateOnNextCall( )
    {
        timeOfCurrentState_ = Time( TUDAT_NAN );
    }

protected:

private:


    //! Current state.
    Eigen::Vector6d currentState_;

    //! Current state with long double precision.
    Eigen::Matrix< long double, 6, 1 > currentLongState_;

    //! Time at which state was last set from ephemeris
    Time timeOfCurrentState_;



    //! Class returning the state of this body's ephemeris origin w.r.t. the global origin (as typically created by
    //! setGlobalFrameBodyEphemerides function).
    boost::shared_ptr< BaseStateInterface > ephemerisFrameToBaseFrame_;

    //! Current rotation from the global to the body-fixed frame.
    Eigen::Quaterniond currentRotationToLocalFrame_;

    //! Current first derivative w.r.t. time of the rotation matrix from the global to the
    //! body-fixed frame.
    Eigen::Matrix3d currentRotationToLocalFrameDerivative_;

    //! Current angular velocity vector for body's rotation, expressed in the global frame.
    Eigen::Vector3d currentAngularVelocityVectorInGlobalFrame_;


    //! Mass of body (default set to zero, calculated from GravityFieldModel when it is set).
    double currentMass_;

    //! Function returning body mass as a function of time.
    boost::function< double( const double ) > bodyMassFunction_;


    //! Ephemeris of body.
    boost::shared_ptr< ephemerides::Ephemeris > bodyEphemeris_;

    //! Gravity field model of body.
    boost::shared_ptr< gravitation::GravityFieldModel > gravityFieldModel_;

    //! Object containing all variations in the gravity field of this body.
    boost::shared_ptr< gravitation::GravityFieldVariationsSet > gravityFieldVariationSet_;

    //! Atmosphere model of body.
    boost::shared_ptr< aerodynamics::AtmosphereModel > atmosphereModel_;

    //! Shape model of body.
    boost::shared_ptr< basic_astrodynamics::BodyShapeModel > shapeModel_;

    //! Aerodynamic coefficient model of body.
    boost::shared_ptr< aerodynamics::AerodynamicCoefficientInterface > aerodynamicCoefficientInterface_;

    //! Object used for calculating current aerodynamic angles, altitude, etc.
    boost::shared_ptr< aerodynamics::FlightConditions > aerodynamicFlightConditions_;

    //! Rotation model of body.
    boost::shared_ptr< ephemerides::RotationalEphemeris > rotationalEphemeris_;

    //! Model to compute the rotation of the body based on the current state of the environment, only valid during propagation.
    boost::shared_ptr< reference_frames::DependentOrientationCalculator > dependentOrientationCalculator_;

    //! List of radiation pressure models for the body, with the sources bodies as key
    std::map< std::string, boost::shared_ptr< electro_magnetism::RadiationPressureInterface > >
            radiationPressureInterfaces_;

    //! Predefined iterator for efficiency purposes.
    std::map< std::string,
              boost::shared_ptr< electro_magnetism::RadiationPressureInterface > >::iterator
    radiationPressureIterator_;

    //! List of ground station objects on Body
    std::map< std::string, boost::shared_ptr< ground_stations::GroundStation > > groundStationMap;

    //! Container object with hardware systems present on/in body (typically only non-NULL for a vehicle).
    boost::shared_ptr< system_models::VehicleSystems > vehicleSystems_;

};

typedef std::unordered_map< std::string, boost::shared_ptr< Body > > NamedBodyMap;

} // namespace simulation_setup

} // namespace tudat

#endif // TUDAT_BODY_H
