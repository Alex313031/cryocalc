#ifndef CRYOCALC_STRINGS_H_
#define CRYOCALC_STRINGS_H_

// Button/Controls titles
inline const wchar_t* convert_button  = L"Convert";
inline const wchar_t* input_label     = L"Input:";
inline const wchar_t* precision_label = L"Precision:";
inline const wchar_t* threads_input   = L"Threads:";
inline const wchar_t* clear_button    = L"Clear";
inline const wchar_t* start_button    = L"Start";
inline const wchar_t* stop_button     = L"Stop";
inline const wchar_t* about_button    = L"About";

#if defined(AFX_TARG_ENU)
 inline const wchar_t* CONV_BUTTON   = convert_button;
 inline const wchar_t* INPUT_LABEL   = input_label;
 inline const wchar_t* PREC_LABEL    = precision_label;
 inline const wchar_t* THREADS_LABEL = threads_input;
 inline const wchar_t* CLEAR_BUTTON  = clear_button;
 inline const wchar_t* START_BUTTON  = start_button;
 inline const wchar_t* STOP_BUTTON   = stop_button;
 inline const wchar_t* ABOUT_BUTTON  = about_button;
#elif defined(AFX_TARG_PH)
 inline const wchar_t* CONV_BUTTON   = L"Magbalik-loob";
 inline const wchar_t* INPUT_LABEL   = L"Temperatura:";
 inline const wchar_t* PREC_LABEL    = L"Katumpakan:";
 inline const wchar_t* THREADS_LABEL = L"Mga Thread:";
 inline const wchar_t* CLEAR_BUTTON  = L"Maaliwalas";
 inline const wchar_t* START_BUTTON  = L"Magsimula";
 inline const wchar_t* STOP_BUTTON   = L"Tumigil ka";
 inline const wchar_t* ABOUT_BUTTON  = L"Tungkol sa";
#elif defined(AFX_TARG_RU)
 inline const wchar_t* CONV_BUTTON   = L"Конвертировать";
 inline const wchar_t* INPUT_LABEL   = L"Вход:";
 inline const wchar_t* PREC_LABEL    = L"Точность:";
 inline const wchar_t* THREADS_LABEL = L"Потоки ЦП:";
 inline const wchar_t* CLEAR_BUTTON  = L"Прозрачный";
 inline const wchar_t* START_BUTTON  = L"Начинать";
 inline const wchar_t* STOP_BUTTON   = L"Останавливаться";
 inline const wchar_t* ABOUT_BUTTON  = L"О программе";
#elif defined(AFX_TARG_ES)
 inline const wchar_t* CONV_BUTTON   = L"Convertir";
 inline const wchar_t* INPUT_LABEL   = L"Aporte:";
 inline const wchar_t* PREC_LABEL    = L"Precisión:";
 inline const wchar_t* THREADS_LABEL = L"Hilos de CPU:";
 inline const wchar_t* CLEAR_BUTTON  = L"Borrar entrada";
 inline const wchar_t* START_BUTTON  = L"Comenzar";
 inline const wchar_t* STOP_BUTTON   = L"Detener";
 inline const wchar_t* ABOUT_BUTTON  = L"Acerca";
#else
 inline const wchar_t* CONV_BUTTON   = convert_button;
 inline const wchar_t* INPUT_LABEL   = input_label;
 inline const wchar_t* PREC_LABEL    = precision_label;
 inline const wchar_t* THREADS_LABEL = threads_input;
 inline const wchar_t* CLEAR_BUTTON  = clear_button;
 inline const wchar_t* START_BUTTON  = start_button;
 inline const wchar_t* STOP_BUTTON   = stop_button;
 inline const wchar_t* ABOUT_BUTTON  = about_button;
#endif

#endif // CRYOCALC_STRINGS_H_
