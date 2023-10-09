
cp $MGC_HOME/shared/include/mc_transactors.h mc_transactors_orig.h
patch mc_transactors_orig.h $SOURCE_DIR/mc_transactors.patch -o mc_transactors.h
