#PBS -N cadscorejob

# Submission form:
#  qsub -v "CADSCORE_JOB_DIR=?,CADSCORE_SCRIPTS_DIR=?,CADSCORE_CALC_OPTIONS=?" CADscore_job_for_PBS.bash

if [ -z "$CADSCORE_JOB_DIR" ] || [ ! -d "$CADSCORE_JOB_DIR" ]
then
  echo "Missing existing job directory" 1>&2
  exit 1
fi

if [ -z "$CADSCORE_SCRIPTS_DIR" ] || [ ! -d "$CADSCORE_SCRIPTS_DIR" ]
then
  echo "Missing existing scripts directory" 1>&2
  exit 1
fi

DATABASE="$CADSCORE_JOB_DIR/database"

true > "$CADSCORE_JOB_DIR/started"

find "$CADSCORE_JOB_DIR/target" -type f | while read TARGET_FILE
do
  find "$CADSCORE_JOB_DIR/model" -type f | while read MODEL_FILE
  do
    $CADSCORE_SCRIPTS_DIR/CADscore_calc.bash -D "$DATABASE" -t "$TARGET_FILE" -m "$MODEL_FILE" $CADSCORE_CALC_OPTIONS
  done
done

$CADSCORE_SCRIPTS_DIR/CADscore_read_table.bash -D "$DATABASE" | column -t > "$DATABASE/table"

true > "$CADSCORE_JOB_DIR/finished"
