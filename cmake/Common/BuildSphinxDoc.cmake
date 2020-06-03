#-------------------------------------------------------------------------------
# Copyright (c) 2019-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#This file adds build and install targets to build the Sphinx documentation
#for TF-M. This currently means the "User Guide". Further documentation
#builds may be added in the future.

Include(CMakeParseArguments)

#This function will find the location of tools needed to build the
#documentation. These are:
#   - Mandatory:
#        - Sphinx 1.9.0 or higher
#        - PlantUML and it's dependencies
#   - Optional
#        - LateX/PDFLateX
#
#Inputs:
#   none (some global variables might be used by FindXXX modules used. For
#         details please check the modules used.)
#Outputs:
#   SPHINX_NODOC - will be defined and set to true is any mandatory tool is
#                  missing.
#   LATEX_PDFLATEX_FOUND - true if pdflatex executable found
#   SPHINX_EXECUTABLE    - path to sphinx-build
#   PLANTUML_JAR_PATH    - path to PlantUML

#Examples
#   SphinxFindTools()
#
function(SphinxFindTools)
	#Find Sphinx
	find_package(Sphinx)

	#Find additional needed Sphinx dependencies.
	find_package(PythonModules COMPONENTS m2r sphinx-rtd-theme sphinxcontrib.plantuml)

	#Find plantUML
	find_package(PlantUML)

	#Find tools needed for PDF generation.
	find_package(LATEX COMPONENTS PDFLATEX)
	set (LATEX_PDFLATEX_FOUND "${LATEX_PDFLATEX_FOUND}" PARENT_SCOPE)

	if (SPHINX_FOUND AND PLANTUML_FOUND AND PY_M2R_FOUND
			AND PY_SPHINX-RTD-THEME_FOUND AND PY_SPHINXCONTRIB.PLANTUML)
		#Export executable locations to global scope.
		set(SPHINX_EXECUTABLE "${SPHINX_EXECUTABLE}" PARENT_SCOPE)
		set(PLANTUML_JAR_PATH "${PLANTUML_JAR_PATH}" PARENT_SCOPE)
		set(Java_JAVA_EXECUTABLE "${Java_JAVA_EXECUTABLE}" PARENT_SCOPE)
		set(SPHINX_NODOC False PARENT_SCOPE)
	else()
		message(WARNING "Some tools are missing for Sphinx document generation. Document generation target is not created.")
		set(SPHINX_NODOC True PARENT_SCOPE)
	endif()
endfunction()

#Find the needed tools.
SphinxFindTools()

#If mandatory tools are missing, skip creating document generation targets.
#This means missing documentation tools is not a critical error, and building
#TF-M is still possible.
if (NOT SPHINX_NODOC)
	#The Sphinx configuration file needs some project specific configuration.
	#Variables with SPHINXCFG_ prefix are setting values related to that.
	#This is where Sphinx output will be written
	set(SPHINXCFG_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/doc_sphinx")

	set(SPHINX_TMP_DOC_DIR "${CMAKE_CURRENT_BINARY_DIR}/doc_sphinx_in")
	set(SPHINXCFG_CONFIGURED_FILE "${TFM_ROOT_DIR}/build_docs/conf.py")
	set(SPHINXCFG_TEMPLATE_FILE "${TFM_ROOT_DIR}/docs/conf.py.in")
	set(SPHINXCFG_ENVIRONMENT_FILE "${TFM_ROOT_DIR}/docs/tfm_env.py.in")
	set(_PDF_FILE "${SPHINXCFG_OUTPUT_PATH}/latex/TF-M.pdf")

	# Set the build-tool to copy over the files to ${SPHINX_TMP_DOC_DIR~
	set(SPHINXCFG_COPY_FILES True)
	# Set the config to render the conf.py. If needed to build it by cmake
	# set it to False
	set(SPHINXCFG_RENDER_CONF True)

	# TODO Reference example on how a doxygen build can be requested.
	# Currently the logic of BuildDoxygenDoc.cmake is still used for
	# compatibility purposes.
	set(DOXYCFG_DOXYGEN_BUILD False)
	if (SPHINXCFG_RENDER_CONF)
		find_package(Doxygen 1.8.0)
		set(DOXYCFG_DOXYGEN_CFG_DIR ${TFM_ROOT_DIR}/doxygen)
		set(DOXYCFG_OUTPUT_PATH ${SPHINXCFG_OUTPUT_PATH}/user_manual)
		set(DOXYCFG_ECLIPSE_DOCID "org.arm.tf-m-refman")
		file(MAKE_DIRECTORY ${SPHINXCFG_OUTPUT_PATH}/user_manual)
	endif()

	#Version ID of TF-M.
	#TODO: this shall not be hard-coded here. We need a process to define the
	#      version number of the document (and TF-M).
	set(SPHINXCFG_TFM_VERSION "v1.1")
	set(SPHINXCFG_TFM_VERSION_FULL "Version 1.1")

	#This command does not generates the specified output file and thus it will
	#always be run. Any other command or target depending on the "run-allways"
	#output will be always executed too.
	add_custom_command(OUTPUT run-allways
		COMMAND "${CMAKE_COMMAND}" -E echo)

	file(REMOVE_RECURSE ${SPHINX_TMP_DOC_DIR})
	file(MAKE_DIRECTORY ${SPHINX_TMP_DOC_DIR})

	#Call configure file to fill out the message template.
	configure_file("${SPHINXCFG_ENVIRONMENT_FILE}" "${SPHINX_TMP_DOC_DIR}/tfm_env.py" @ONLY)

	file(COPY "${SPHINXCFG_CONFIGURED_FILE}" DESTINATION ${SPHINX_TMP_DOC_DIR})

	add_custom_target(create_sphinx_input
		SOURCES "${SPHINX_TMP_DOC_DIR}"
	)

	add_custom_command(OUTPUT "${SPHINXCFG_OUTPUT_PATH}/html"
		COMMAND "${SPHINX_EXECUTABLE}" -b html "${SPHINX_TMP_DOC_DIR}" "${SPHINXCFG_OUTPUT_PATH}/html"
		WORKING_DIRECTORY "${TFM_ROOT_DIR}"
		DEPENDS create_sphinx_input run-allways
		COMMENT "Running Sphinx to generate user guide (HTML)."
		VERBATIM
		)

	#Create build target to generate HTML documentation.
	add_custom_target(doc_userguide
		COMMENT "Generating User Guide with Sphinx..."
		#Copy document files from the top level dir to docs
		SOURCES "${SPHINXCFG_OUTPUT_PATH}/html"
		)

	#Add the HTML documentation to install content
	install(DIRECTORY ${SPHINXCFG_OUTPUT_PATH}/html DESTINATION doc/user_guide
		EXCLUDE_FROM_ALL
		COMPONENT user_guide
		PATTERN .buildinfo EXCLUDE
		)

	if (DOXYCFG_DOXYGEN_BUILD)
		#Add the HTML documentation to install content
		install(DIRECTORY ${SPHINXCFG_OUTPUT_PATH}/user_manual DESTINATION doc
		EXCLUDE_FROM_ALL
		COMPONENT user_guide
		PATTERN .buildinfo EXCLUDE
		)
	endif()

	#If PDF documentation is being made.
	if (LATEX_PDFLATEX_FOUND)
		if (NOT CMAKE_GENERATOR MATCHES "Makefiles")
			message(WARNING "Generator is not make based. PDF document generation target is not created.")
		else()
			#This file shall not be included before cmake did finish finding the make tool and thus
			#setting CMAKE_MAKE_PROGRAM. Currently the search is triggered by the project() command.
			if(NOT CMAKE_MAKE_PROGRAM)
				message(FATAL_ERROR "CMAKE_MAKE_PROGRAM is not set. This file must be included after the project command is run.")
			endif()

			add_custom_command(OUTPUT "${SPHINXCFG_OUTPUT_PATH}/latex"
				COMMAND "${SPHINX_EXECUTABLE}" -b latex "${SPHINX_TMP_DOC_DIR}" "${SPHINXCFG_OUTPUT_PATH}/latex"
				WORKING_DIRECTORY "${TFM_ROOT_DIR}"
				DEPENDS create_sphinx_input
				COMMENT "Running Sphinx to generate user guide (LaTeX)."
				VERBATIM
				)

			add_custom_command(OUTPUT "${_PDF_FILE}"
				COMMAND "${CMAKE_MAKE_PROGRAM}" all-pdf
				WORKING_DIRECTORY ${SPHINXCFG_OUTPUT_PATH}/latex
				DEPENDS "${SPHINXCFG_OUTPUT_PATH}/latex"
				COMMENT "Generating PDF version of User Guide..."
				VERBATIM
				)

			#We do not use the add_custom_command trick here to get proper clean
			#command since the clean rules "added" above will remove the entire
			#doc directory, and thus clean the PDF output too.
			add_custom_target(doc_userguide_pdf
				COMMENT "Generating PDF version of TF-M User Guide..."
				SOURCES "${_PDF_FILE}"
				VERBATIM)

			#Add the pdf documentation to install content
			install(FILES "${_PDF_FILE}" DESTINATION "doc/user_guide"
				RENAME "tf-m_user_guide.pdf"
				COMPONENT user_guide
				EXCLUDE_FROM_ALL)
		endif()
	else()
		message(WARNING "PDF generation tools are missing. PDF document generation target is not created.")
	endif()

	#Generate build target which installs the documentation.
	if (TARGET doc_userguide_pdf)
		add_custom_target(install_userguide
			DEPENDS doc_userguide doc_userguide_pdf
			COMMAND "${CMAKE_COMMAND}" -DCMAKE_INSTALL_COMPONENT=user_guide
			-P "${CMAKE_BINARY_DIR}/cmake_install.cmake")
	else()
		add_custom_target(install_userguide
			DEPENDS doc_userguide
			COMMAND "${CMAKE_COMMAND}" -DCMAKE_INSTALL_COMPONENT=user_guide
			-P "${CMAKE_BINARY_DIR}/cmake_install.cmake")
	endif()

endif()
