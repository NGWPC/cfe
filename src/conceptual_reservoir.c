#ifndef _CONCEPTUAL_RESERVOIR_C
#define _CONCEPTUAL_RESERVOIR_C

#include "logger.h"
#include "conceptual_reservoir.h"


//##############################################################
//########## SINGLE OUTLET EXPONENTIAL RESERVOIR ###############
//##########                -or-                 ###############
//##########    TWO OUTLET NONLINEAR RESERVOIR   ###############
//################################################################
// This function calculates the flux from a linear, or nonlinear 
// conceptual reservoir with one or two outlets, or from an
// exponential nonlinear conceptual reservoir with only one outlet.
// In the non-exponential instance, each outlet can have its own
// activation storage threshold.  Flow from the second outlet is 
// turned off by setting the discharge coeff. to 0.0.
//################################################################
extern void conceptual_reservoir_flux_calc(struct conceptual_reservoir *da_reservoir,
                                           double *primary_flux_m,double *secondary_flux_m)
{
  //struct conceptual_reservoir  <<<<INCLUDED HERE FOR REFERENCE.>>>>
  //{
  // int    is_exponential;  // set this true TRUE to use the exponential form of the discharge equation
  // double storage_max_m;
  // double storage_m;
  // double coeff_primary;
  // double exponent_secondary;
  // double storage_threshold_primary_m;
  // double storage_threshold_secondary_m;
  // double coeff_secondary;
  // double exponent_secondary;
  // };
  // THIS FUNCTION CALCULATES THE FLUXES FROM A CONCEPTUAL NON-LINEAR (OR LINEAR) RESERVOIR WITH TWO OUTLETS
  // all fluxes calculated by this routine are instantaneous with units of the coefficient.
  
  //local variables
  double storage_above_threshold_m;

  // *****************************************************************************
  // ------------------ Conceptual Ground Water Reservoir -------------------------
  // single outlet reservoir like the NWM V1.2 exponential conceptual gw reservoir
  if (da_reservoir->is_exponential == TRUE) {
    *primary_flux_m = 0.0;
    *secondary_flux_m = 0.0;

    static int gw_bad_state_warned = 0;

    if (!isfinite(da_reservoir->storage_m) ||
        !isfinite(da_reservoir->storage_max_m) ||
        !isfinite(da_reservoir->coeff_primary) ||
        !isfinite(da_reservoir->exponent_primary) ||
        da_reservoir->storage_m <= 0.0 ||
        da_reservoir->storage_max_m <= 0.0) {

      if (gw_bad_state_warned == 0) {
        Log(WARNING,
            "Invalid CFE groundwater reservoir state or parameters during flux calculation; "
            "returning zero groundwater flux for this timestep. "
            "storage_m=%lf, storage_max_m=%lf, Cgw=%lf, expon=%lf.",
            da_reservoir->storage_m,
            da_reservoir->storage_max_m,
            da_reservoir->coeff_primary,
            da_reservoir->exponent_primary);

        gw_bad_state_warned = 1;
      }
      else if (gw_bad_state_warned == 1) {
        Log(WARNING,
            "Invalid CFE groundwater reservoir state or parameters occurred again; "
            "subsequent occurrences of this message will be suppressed.");

        gw_bad_state_warned = 2;
      }

      return;
    }

    // calculate the one flux and return.
    double exp_term = exp(da_reservoir->exponent_primary * da_reservoir->storage_m / da_reservoir->storage_max_m);
    double calculated_flux_m = da_reservoir->coeff_primary * (exp_term - 1.0);

    if (!isfinite(calculated_flux_m) || calculated_flux_m < 0.0) {
      static int gw_bad_flux_warned = 0;

      if (gw_bad_flux_warned == 0) {
        Log(WARNING,
            "Invalid CFE groundwater flux calculation; "
            "returning zero groundwater flux for this timestep. "
            "storage_m=%lf, storage_max_m=%lf, Cgw=%lf, expon=%lf, raw_flux=%lf.",
            da_reservoir->storage_m,
            da_reservoir->storage_max_m,
            da_reservoir->coeff_primary,
            da_reservoir->exponent_primary,
            calculated_flux_m);

        gw_bad_flux_warned = 1;
      }
      else if (gw_bad_flux_warned == 1) {
        Log(WARNING,
            "Invalid CFE groundwater flux calculation occurred again; "
            "subsequent occurrences of this message will be suppressed.");

        gw_bad_flux_warned = 2;
      }

      return;
    }

    *primary_flux_m = calculated_flux_m;

    // cap so flux never exceeds storage, but commented out here per OWP request (9/11/2025)
    //if (*primary_flux_m > da_reservoir->storage_m) {
    //    *primary_flux_m = da_reservoir->storage_m;
    //}

    return;
  }

  // *****************************************************************************
  
  // code goes past here iff it is not a single outlet exponential deep groundwater reservoir of the NWM variety
  // The vertical outlet is assumed to be primary and satisfied first.
  
  *primary_flux_m=0.0;
  storage_above_threshold_m = da_reservoir->storage_m - da_reservoir->storage_threshold_primary_m;
  
  if (storage_above_threshold_m > 0.0) {
    // flow is possible from the primary outlet
    *primary_flux_m = da_reservoir->coeff_primary *
      pow(storage_above_threshold_m / (da_reservoir->storage_max_m - da_reservoir->storage_threshold_primary_m),
	  da_reservoir->exponent_primary);
    
    if(*primary_flux_m > storage_above_threshold_m) 
      *primary_flux_m = storage_above_threshold_m;  // limit to max. available
  }
  
  *secondary_flux_m=0.0;
  storage_above_threshold_m = da_reservoir->storage_m - da_reservoir->storage_threshold_secondary_m;

  if (storage_above_threshold_m > 0.0) {
    // flow is possible from the secondary outlet
    *secondary_flux_m = da_reservoir->coeff_secondary *
      pow(storage_above_threshold_m / (da_reservoir->storage_max_m - da_reservoir->storage_threshold_secondary_m),
	  da_reservoir->exponent_secondary);
    
    if (*secondary_flux_m > (storage_above_threshold_m-(*primary_flux_m))) 
      *secondary_flux_m = storage_above_threshold_m-(*primary_flux_m);  // limit to max. available
  }
  return;
}


#endif
