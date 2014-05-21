#NEED TO DEFINE VARIABLES $(LOCAL_PATH) AND $(SRC_FILES_TO_EXCLUDE)

#----this function return all files recoursively in any depth-----
wildcard_recoursive = \
          $(subst $(LOCAL_PATH)/,,$(wildcard $1$2)) \
          $(foreach d,$(wildcard $1*), \
	       $(call wildcard_recoursive,$d/,$2) \
          )

#----return $1 if need to exclude, otherwise return empty string---
is_file_is_need_to_exclude = \
          $(strip \
              $(foreach exclude_file,$(SRC_FILES_TO_EXCLUDE), \
                  $(if $(filter %$(exclude_file),$1),$1,) \
              ) \
          )

#---exclude files from list---
exclude_files_from_list = \
          $(foreach f,$1, \
		$(if $(call is_file_is_need_to_exclude,$f),,$f) \
          )

#---add all c and cpp files from path---
all_source_files_from_path = \
          $(call exclude_files_from_list,$(call wildcard_recoursive,$1,*.cpp)) \
          $(call exclude_files_from_list,$(call wildcard_recoursive,$1,*.c))


#-------SOME_TESTS-------
#$(info path = $(wildcard $(LOCAL_PATH)/../../Classes/*/.))
#$(info path = $(call wildcard_recoursive,$(LOCAL_PATH)/../../Classes/,*.cpp))
#$(info path=$(call is_file_is_need_to_exclude,ss/lua.c))
#$(info path=$(call exclude_files_from_list,ss/lua.c sss.c asdasd/asdasd/LuaPlusAll.cpp aaa.c))
#$(info path = $(call all_source_files_from_path,$(LOCAL_PATH)/../../lib/LuaPlus/))