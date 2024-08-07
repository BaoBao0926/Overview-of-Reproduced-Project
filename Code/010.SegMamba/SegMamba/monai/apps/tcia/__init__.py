# Copyright (c) MONAI Consortium
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from __future__ import annotations

from .label_desc import TCIA_LABEL_DICT
from .utils import (
    BASE_URL,
    DCM_FILENAME_REGEX,
    download_tcia_series_instance,
    get_tcia_metadata,
    get_tcia_ref_uid,
    match_tcia_ref_uid_in_study,
)
