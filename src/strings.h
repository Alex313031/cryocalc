#ifndef CRYOCALC_STRINGS_H_
#define CRYOCALC_STRINGS_H_

// Button/Controls titles
constexpr wchar_t convert_button[]  = L"Convert";
constexpr wchar_t input_label[]     = L"Input:";
constexpr wchar_t precision_label[] = L"Precision:";
constexpr wchar_t threads_input[]   = L"Threads:";
constexpr wchar_t clear_button[]    = L"Clear";
constexpr wchar_t start_button[]    = L"Start";
constexpr wchar_t stop_button[]     = L"Stop";
constexpr wchar_t about_button[]    = L"About";
constexpr wchar_t osinfo_button[]   = L"Show OS Info";
constexpr wchar_t close_oi_button[] = L"Close OS Info";
constexpr wchar_t osinfo_title[]    = L"OS Info";
constexpr wchar_t winver_button[]   = L"Run Winver.exe";
constexpr wchar_t msinfo_button[]   = L"Run MSInfo32.exe";
constexpr wchar_t run_button[]      = L"Run Program";
constexpr wchar_t run_title[]       = L"Create New Task";
constexpr wchar_t run_prompt[]      = L"Type the name of a program, folder, or file, and CryoCalc will open it for you.";
constexpr wchar_t use_sse2q[]       = L"Use SSE2";
constexpr wchar_t cache_multi[]     = L"Cache ";
constexpr wchar_t alloc_memory[]    = L"Allocate 100MB";

#if defined(AFX_TARG_ENU)
 inline const wchar_t* CONV_BUTTON   = convert_button;
 inline const wchar_t* INPUT_LABEL   = input_label;
 inline const wchar_t* PREC_LABEL    = precision_label;
 inline const wchar_t* THREADS_LABEL = threads_input;
 inline const wchar_t* CLEAR_BUTTON  = clear_button;
 inline const wchar_t* START_BUTTON  = start_button;
 inline const wchar_t* STOP_BUTTON   = stop_button;
 inline const wchar_t* ABOUT_BUTTON  = about_button;
 inline const wchar_t* OSINFO_TITLE  = osinfo_title;
 inline const wchar_t* OSINFO_BUTTON = osinfo_button;
 inline const wchar_t* CLOSE_OI_BUTT = close_oi_button;
 inline const wchar_t* WINVER_BUTTON = winver_button;
 inline const wchar_t* MSINFO_BUTTON = msinfo_button;
 inline const wchar_t* RUN_BUTTON    = run_button;
 inline const wchar_t* RUN_TITLE     = run_title;
 inline const wchar_t* RUN_PROMPT    = run_prompt;
 inline const wchar_t* USE_SSE2Q     = use_sse2q;
 inline const wchar_t* CACHE_SIZEQ   = cache_multi;
 inline const wchar_t* ALLOC_MEM     = alloc_memory;
#elif defined(AFX_TARG_PH)
 inline const wchar_t* CONV_BUTTON   = L"Magbalik-loob";
 inline const wchar_t* INPUT_LABEL   = L"Temperatura:";
 inline const wchar_t* PREC_LABEL    = L"Katumpakan:";
 inline const wchar_t* THREADS_LABEL = L"Mga Thread:";
 inline const wchar_t* CLEAR_BUTTON  = L"Maaliwalas";
 inline const wchar_t* START_BUTTON  = L"Magsimula";
 inline const wchar_t* STOP_BUTTON   = L"Tumigil ka";
 inline const wchar_t* ABOUT_BUTTON  = L"Tungkol sa";
 inline const wchar_t* OSINFO_TITLE  = osinfo_title;
 inline const wchar_t* OSINFO_BUTTON = osinfo_button;
 inline const wchar_t* CLOSE_OI_BUTT = close_oi_button;
 inline const wchar_t* WINVER_BUTTON = winver_button;
 inline const wchar_t* MSINFO_BUTTON = msinfo_button;
 inline const wchar_t* RUN_BUTTON    = run_button;
 inline const wchar_t* RUN_TITLE     = run_title;
 inline const wchar_t* RUN_PROMPT    = run_prompt;
 inline const wchar_t* USE_SSE2Q     = use_sse2q;
 inline const wchar_t* CACHE_SIZEQ   = cache_multi;
 inline const wchar_t* ALLOC_MEM     = alloc_memory;
#elif defined(AFX_TARG_RU)
 inline const wchar_t* CONV_BUTTON   = L"Конвертировать";
 inline const wchar_t* INPUT_LABEL   = L"Вход:";
 inline const wchar_t* PREC_LABEL    = L"Точность:";
 inline const wchar_t* THREADS_LABEL = L"Потоки ЦП:";
 inline const wchar_t* CLEAR_BUTTON  = L"Прозрачный";
 inline const wchar_t* START_BUTTON  = L"Начинать";
 inline const wchar_t* STOP_BUTTON   = L"Останавливаться";
 inline const wchar_t* ABOUT_BUTTON  = L"О программе";
 inline const wchar_t* OSINFO_TITLE  = osinfo_title;
 inline const wchar_t* OSINFO_BUTTON = osinfo_button;
 inline const wchar_t* CLOSE_OI_BUTT = close_oi_button;
 inline const wchar_t* WINVER_BUTTON = winver_button;
 inline const wchar_t* MSINFO_BUTTON = msinfo_button;
 inline const wchar_t* RUN_BUTTON    = run_button;
 inline const wchar_t* RUN_TITLE     = run_title;
 inline const wchar_t* RUN_PROMPT    = run_prompt;
 inline const wchar_t* USE_SSE2Q     = use_sse2q;
 inline const wchar_t* CACHE_SIZEQ   = cache_multi;
 inline const wchar_t* ALLOC_MEM     = alloc_memory;
#elif defined(AFX_TARG_ES)
 inline const wchar_t* CONV_BUTTON   = L"Convertir";
 inline const wchar_t* INPUT_LABEL   = L"Aporte:";
 inline const wchar_t* PREC_LABEL    = L"Precisión:";
 inline const wchar_t* THREADS_LABEL = L"Hilos de CPU:";
 inline const wchar_t* CLEAR_BUTTON  = L"Borrar entrada";
 inline const wchar_t* START_BUTTON  = L"Comenzar";
 inline const wchar_t* STOP_BUTTON   = L"Detener";
 inline const wchar_t* ABOUT_BUTTON  = L"Acerca";
 inline const wchar_t* OSINFO_TITLE  = osinfo_title;
 inline const wchar_t* OSINFO_BUTTON = osinfo_button;
 inline const wchar_t* CLOSE_OI_BUTT = close_oi_button;
 inline const wchar_t* WINVER_BUTTON = winver_button;
 inline const wchar_t* MSINFO_BUTTON = msinfo_button;
 inline const wchar_t* RUN_BUTTON    = run_button;
 inline const wchar_t* RUN_TITLE     = run_title;
 inline const wchar_t* RUN_PROMPT    = run_prompt;
 inline const wchar_t* USE_SSE2Q     = use_sse2q;
 inline const wchar_t* CACHE_SIZEQ   = cache_multi;
 inline const wchar_t* ALLOC_MEM     = alloc_memory;
#else
 // Fallback to English
 inline const wchar_t* CONV_BUTTON   = convert_button;
 inline const wchar_t* INPUT_LABEL   = input_label;
 inline const wchar_t* PREC_LABEL    = precision_label;
 inline const wchar_t* THREADS_LABEL = threads_input;
 inline const wchar_t* CLEAR_BUTTON  = clear_button;
 inline const wchar_t* START_BUTTON  = start_button;
 inline const wchar_t* STOP_BUTTON   = stop_button;
 inline const wchar_t* ABOUT_BUTTON  = about_button;
 inline const wchar_t* OSINFO_TITLE  = osinfo_title;
 inline const wchar_t* OSINFO_BUTTON = osinfo_button;
 inline const wchar_t* CLOSE_OI_BUTT = close_oi_button;
 inline const wchar_t* WINVER_BUTTON = winver_button;
 inline const wchar_t* MSINFO_BUTTON = msinfo_button;
 inline const wchar_t* RUN_BUTTON    = run_button;
 inline const wchar_t* RUN_TITLE     = run_title;
 inline const wchar_t* RUN_PROMPT    = run_prompt;
 inline const wchar_t* USE_SSE2Q     = use_sse2q;
 inline const wchar_t* CACHE_SIZEQ   = cache_multi;
 inline const wchar_t* ALLOC_MEM     = alloc_memory;
#endif

#endif // CRYOCALC_STRINGS_H_
